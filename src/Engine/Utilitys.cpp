#include <iostream>
#include <filesystem>
#include <string>
#include <random>
#include <chrono>

namespace fs = std::filesystem;

/**
 * @brief Creates a unique temporary folder in the user's temp directory.
 *
 * This function generates a unique folder name using a combination of a prefix
 * and random characters. If the folder already exists, it returns the existing path.
 *
 * @return fs::path The path to the created or existing temporary folder.
 * @throws fs::filesystem_error if directory creation fails.
 */
std::filesystem::path createTempFolder() {
    const std::string folder_name = "temp_tesseract_fixed";
    
    // Get the system's temporary directory
    fs::path temp_dir = fs::temp_directory_path();
    
    // Define the fixed folder path
    fs::path fixed_folder = temp_dir / folder_name;
    
    // Check if the folder exists
    if (fs::exists(fixed_folder)) {
        if (fs::is_directory(fixed_folder)) {
            // Folder already exists; return its path
            return fixed_folder;
        } else {
            // A file with the same name exists; handle the conflict
            throw fs::filesystem_error("A file with the fixed folder name exists.",
                                       fixed_folder,
                                       std::error_code());
        }
    }
    
    // Attempt to create the directory
    try {
        if (fs::create_directory(fixed_folder)) {
            // Successfully created the folder
            return fixed_folder;
        } else {
            // Failed to create the folder for an unknown reason
            throw fs::filesystem_error("Failed to create the fixed temporary folder.",
                                       fixed_folder,
                                       std::error_code());
        }
    } catch (const fs::filesystem_error& e) {
        // Handle filesystem errors (e.g., permission issues)
        std::cerr << "Error creating directory: " << e.what() << '\n';
        throw; // Re-throw the exception after logging
    }
}


