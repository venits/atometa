#include "Atometa/Chemistry/MolecularFileIO.h"
#include "Atometa/Chemistry/PeriodicTable.h"
#include "Atometa/Core/Logger.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace Atometa {

    // ========================================================================
    // XYZ Format
    // ========================================================================

    bool MolecularFileIO::LoadXYZ(const std::string& filepath, Molecule& molecule) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            ATOMETA_ERROR("Failed to open file: ", filepath);
            return false;
        }

        std::string line;
        
        // Read atom count
        int atomCount = 0;
        if (!std::getline(file, line)) {
            ATOMETA_ERROR("Invalid XYZ file: missing atom count");
            return false;
        }
        std::istringstream(line) >> atomCount;

        // Read comment line
        std::string comment;
        if (!std::getline(file, comment)) {
            ATOMETA_ERROR("Invalid XYZ file: missing comment line");
            return false;
        }
        molecule.SetName(comment);

        // Read atoms
        for (int i = 0; i < atomCount; ++i) {
            if (!std::getline(file, line)) {
                ATOMETA_ERROR("Unexpected end of file at atom ", i);
                return false;
            }

            std::istringstream iss(line);
            std::string symbol;
            float x, y, z;

            if (!(iss >> symbol >> x >> y >> z)) {
                ATOMETA_ERROR("Invalid atom data at line ", i + 3);
                return false;
            }

            // Get element from periodic table
            if (PeriodicTable::IsValidElement(symbol)) {
                const ElementData& element = PeriodicTable::GetElement(symbol);
                AtomType type = static_cast<AtomType>(element.AtomicNumber);
                molecule.AddAtom(type, glm::vec3(x, y, z));
            } else {
                ATOMETA_WARN("Unknown element: ", symbol, ", using Carbon");
                molecule.AddAtom(AtomType::Carbon, glm::vec3(x, y, z));
            }
        }

        ATOMETA_INFO("Loaded ", atomCount, " atoms from ", filepath);
        return true;
    }

    bool MolecularFileIO::SaveXYZ(const std::string& filepath, const Molecule& molecule) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            ATOMETA_ERROR("Failed to create file: ", filepath);
            return false;
        }

        const auto& atoms = molecule.GetAtoms();

        // Write atom count
        file << atoms.size() << "\n";

        // Write comment (molecule name)
        file << molecule.GetName() << "\n";

        // Write atoms
        for (const auto& atom : atoms) {
            file << atom.Symbol << " "
                 << atom.Position.x << " "
                 << atom.Position.y << " "
                 << atom.Position.z << "\n";
        }

        ATOMETA_INFO("Saved ", atoms.size(), " atoms to ", filepath);
        return true;
    }

    // ========================================================================
    // PDB Format (Simplified)
    // ========================================================================

    bool MolecularFileIO::LoadPDB(const std::string& filepath, Molecule& molecule) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            ATOMETA_ERROR("Failed to open file: ", filepath);
            return false;
        }

        std::string line;
        int atomCount = 0;

        while (std::getline(file, line)) {
            if (line.substr(0, 6) == "ATOM  " || line.substr(0, 6) == "HETATM") {
                // PDB format:
                // ATOM      1  N   ALA A   1      11.104   6.134  -6.504  1.00  0.00           N

                if (line.length() < 54) continue;

                try {
                    std::string symbol = line.substr(76, 2);
                    // Trim whitespace
                    symbol.erase(0, symbol.find_first_not_of(" \t"));
                    symbol.erase(symbol.find_last_not_of(" \t") + 1);

                    if (symbol.empty()) {
                        // Try atom name column
                        symbol = line.substr(12, 4);
                        symbol.erase(0, symbol.find_first_not_of(" \t"));
                        symbol.erase(symbol.find_last_not_of(" \t") + 1);
                        // Take first non-digit character
                        symbol = symbol.substr(0, 1);
                    }

                    float x = std::stof(line.substr(30, 8));
                    float y = std::stof(line.substr(38, 8));
                    float z = std::stof(line.substr(46, 8));

                    if (PeriodicTable::IsValidElement(symbol)) {
                        const ElementData& element = PeriodicTable::GetElement(symbol);
                        AtomType type = static_cast<AtomType>(element.AtomicNumber);
                        molecule.AddAtom(type, glm::vec3(x, y, z));
                        atomCount++;
                    } else {
                        ATOMETA_WARN("Unknown element in PDB: ", symbol);
                    }
                } catch (const std::exception& e) {
                    ATOMETA_WARN("Error parsing PDB line: ", e.what());
                    continue;
                }
            }
            else if (line.substr(0, 6) == "HEADER") {
                if (line.length() > 10) {
                    std::string name = line.substr(10);
                    // Trim whitespace
                    name.erase(0, name.find_first_not_of(" \t"));
                    name.erase(name.find_last_not_of(" \t") + 1);
                    molecule.SetName(name);
                }
            }
        }

        ATOMETA_INFO("Loaded ", atomCount, " atoms from PDB file: ", filepath);
        return atomCount > 0;
    }

    bool MolecularFileIO::SavePDB(const std::string& filepath, const Molecule& molecule) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            ATOMETA_ERROR("Failed to create file: ", filepath);
            return false;
        }

        const auto& atoms = molecule.GetAtoms();

        // Write header
        file << "HEADER    " << molecule.GetName() << "\n";
        file << "REMARK    Generated by Atometa\n";

        // Write atoms
        for (size_t i = 0; i < atoms.size(); ++i) {
            const auto& atom = atoms[i];

            char line[81];
            snprintf(line, sizeof(line),
                "ATOM  %5zu  %-4s MOL A   1    %8.3f%8.3f%8.3f  1.00  0.00          %2s",
                i + 1,
                atom.Symbol.c_str(),
                atom.Position.x,
                atom.Position.y,
                atom.Position.z,
                atom.Symbol.c_str());

            file << line << "\n";
        }

        file << "END\n";

        ATOMETA_INFO("Saved ", atoms.size(), " atoms to PDB file: ", filepath);
        return true;
    }

    // ========================================================================
    // Auto-detect Format
    // ========================================================================

    bool MolecularFileIO::Load(const std::string& filepath, Molecule& molecule) {
        std::string ext = GetFileExtension(filepath);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".xyz") {
            return LoadXYZ(filepath, molecule);
        } else if (ext == ".pdb") {
            return LoadPDB(filepath, molecule);
        } else {
            ATOMETA_ERROR("Unsupported file format: ", ext);
            return false;
        }
    }

    bool MolecularFileIO::Save(const std::string& filepath, const Molecule& molecule) {
        std::string ext = GetFileExtension(filepath);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".xyz") {
            return SaveXYZ(filepath, molecule);
        } else if (ext == ".pdb") {
            return SavePDB(filepath, molecule);
        } else {
            ATOMETA_ERROR("Unsupported file format: ", ext);
            return false;
        }
    }

    std::vector<std::string> MolecularFileIO::GetSupportedFormats() {
        return {".xyz", ".pdb"};
    }

    std::string MolecularFileIO::GetFormatDescription(const std::string& extension) {
        std::string ext = extension;
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".xyz") {
            return "XYZ Cartesian Coordinates";
        } else if (ext == ".pdb") {
            return "Protein Data Bank Format";
        } else {
            return "Unknown Format";
        }
    }

    std::string MolecularFileIO::GetFileExtension(const std::string& filepath) {
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos == std::string::npos) {
            return "";
        }
        return filepath.substr(dotPos);
    }

}