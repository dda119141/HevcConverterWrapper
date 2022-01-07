/*
 * Maxime Moge dda119141@gmail.com 05.07.2021
 * main.cpp : This file contains the 'main' function. Program execution begins and ends there.
 *
*/

#include <parse_files.hpp>
#include <experimental/filesystem>
#include <lyra/lyra.hpp>


struct convertOptions {
    bool nr_of_parralel_conversions;
    bool to_delete;
} convertOption = {};


bool parse_files(const std::string& directory,
                           const struct convertOptions& tags) {
    namespace fs = std::experimental::filesystem;

    using std::cout;
    using std::endl;

    const std::string currentFilePath = fs::absolute(directory).string();

    const fs::path mp3Path = fs::path(currentFilePath);
    if (!fs::exists(mp3Path)) {
        std::cerr << "Path: " << currentFilePath << " does not exist" << endl;

        return false;

    } else if (fs::is_regular_file(mp3Path)) {
        const std::string mediafile = mp3Path.string();

        process_file(mediafile);
    } else {
        try {
            for (auto& filen : fs::recursive_directory_iterator(mp3Path.string())) {
                const std::string mediafile = filen.path().string();

                process_file(mediafile);
            }

        } catch (fs::filesystem_error& e) {
            cout << "wrong path:" << e.what() << endl;
        }
    }

    return true;
}


int main(int argc, const char** argv) {

    bool show_help = false;
    std::string directory;

    auto parser =
        lyra::help(show_help).description(
                "This is an utility for converting video files to x265 format \nwithin "
                "a directory.") |
        lyra::opt(directory, "dir")["--directory"]["-d"](
                "specify directory or file to use for convertion (REQUIRED).")
        .required() |
        lyra::opt(convertOption.to_delete)["--delete"]["-d"]("delete original file upon successful convertion.") |
        lyra::opt(convertOption.nr_of_parralel_conversions)["--nr_of_parralel_conversions"]["-l"]("number of parralel execution actions. (efficient usage of the existing processor cores");

    // Parse the program arguments:
    auto result = parser.parse({argc, argv});

    // Check that the arguments where valid:
    if (!result) {
        std::cerr << "Error in command line: " << result.errorMessage()
            << std::endl;
        std::cerr << parser << std::endl;

        std::exit(1);
    } else if(show_help) {

        std::cout << parser << std::endl;
    }else{
        parse_files(directory, convertOption);

    }

}


