#include "Atometa/Renderer/ModelLoader.h"
#include "Atometa/Core/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

namespace Atometa {

    // ── Public ─────────────────────────────────────────────────────────────

    LoadedModel ModelLoader::Load(const std::string& filepath)
    {
        LoadedModel result;
        result.SourcePath = filepath;

        Assimp::Importer importer;

        // Post-process flags:
        //   Triangulate     — convert quads/polygons to triangles
        //   GenSmoothNormals — generate normals if absent
        //   CalcTangentSpace — fill Tangent/Bitangent for normal mapping
        //   FlipUVs          — OpenGL expects V flipped vs most exporters
        //   JoinIdenticalVertices — deduplicate vertices
        const unsigned int flags =
            aiProcess_Triangulate          |
            aiProcess_GenSmoothNormals     |
            aiProcess_CalcTangentSpace     |
            aiProcess_FlipUVs              |
            aiProcess_JoinIdenticalVertices;

        const aiScene* scene = importer.ReadFile(filepath, flags);

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
        {
            ATOMETA_ERROR("ModelLoader: failed to load '", filepath, "' — ", importer.GetErrorString());
            return result;
        }

        result.SubMeshes.reserve(scene->mNumMeshes);

        // Walk every mesh in the scene (flat — no node transform applied yet)
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
            result.SubMeshes.push_back(ProcessMesh(scene->mMeshes[i], scene));

        result.Success = true;
        ATOMETA_INFO("ModelLoader: loaded '", filepath, "' — ",
                     result.SubMeshes.size(), " submesh(es)");

        return result;
    }

    // ── Private ────────────────────────────────────────────────────────────

    SubMesh ModelLoader::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
    {
        SubMesh subMesh;
        subMesh.Name = mesh->mName.C_Str();

        // ── Vertices ───────────────────────────────────────────────────────
        std::vector<Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex v;

            // Position
            v.Position = { mesh->mVertices[i].x,
                           mesh->mVertices[i].y,
                           mesh->mVertices[i].z };

            // Normal
            if (mesh->HasNormals())
                v.Normal = { mesh->mNormals[i].x,
                             mesh->mNormals[i].y,
                             mesh->mNormals[i].z };

            // UV (first channel only)
            if (mesh->mTextureCoords[0])
                v.TexCoords = { mesh->mTextureCoords[0][i].x,
                                mesh->mTextureCoords[0][i].y };

            // Tangent / Bitangent
            if (mesh->HasTangentsAndBitangents())
            {
                v.Tangent   = { mesh->mTangents[i].x,
                                mesh->mTangents[i].y,
                                mesh->mTangents[i].z };
                v.Bitangent = { mesh->mBitangents[i].x,
                                mesh->mBitangents[i].y,
                                mesh->mBitangents[i].z };
            }

            vertices.push_back(v);
        }

        // ── Indices ────────────────────────────────────────────────────────
        std::vector<uint32_t> indices;
        indices.reserve(mesh->mNumFaces * 3);

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        // ── Material ───────────────────────────────────────────────────────
        MeshMaterial mat;
        if (mesh->mMaterialIndex < scene->mNumMaterials)
        {
            const aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
            mat.Name = aiMat->GetName().C_Str();

            // Base color / diffuse
            aiColor4D color;
            if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
                mat.BaseColor = { color.r, color.g, color.b };

            // PBR metallic/roughness (GLB/glTF)
            float metallic = 0.f, roughness = 0.5f;
            aiMat->Get(AI_MATKEY_METALLIC_FACTOR,   metallic);
            aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR,  roughness);
            mat.Metallic   = metallic;
            mat.Roughness  = roughness;
        }

        subMesh.Geometry = Mesh(vertices, indices);
        subMesh.Material = mat;
        return subMesh;
    }

} // namespace Atometa