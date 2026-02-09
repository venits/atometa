#pragma once

#include "Atometa/Core/Core.h"
#include "Molecule.h"
#include <string>

namespace Atometa {

    class MolecularFileIO {
    public:
        // XYZ format
        static bool LoadXYZ(const std::string& filepath, Molecule& molecule);
        static bool SaveXYZ(const std::string& filepath, const Molecule& molecule);

        // PDB format
        static bool LoadPDB(const std::string& filepath, Molecule& molecule);
        static bool SavePDB(const std::string& filepath, const Molecule& molecule);

        // Auto-detect format from extension
        static bool Load(const std::string& filepath, Molecule& molecule);
        static bool Save(const std::string& filepath, const Molecule& molecule);

        // Get supported formats
        static std::vector<std::string> GetSupportedFormats();
        static std::string GetFormatDescription(const std::string& extension);

    private:
        static std::string GetFileExtension(const std::string& filepath);
    };

}