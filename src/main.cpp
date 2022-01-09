/*
 * Maxime Moge dda119141@gmail.com 05.07.2021
 * main.cpp : This file contains the 'main' function. Program execution begins and ends there.
 *
*/

#include <parse_files.hpp>
#include <experimental/filesystem>
#include <algorithm>
#include <execution>
#include <lyra/lyra.hpp>

struct convertOptions
{
    int nr_of_parallel_conversions = 1;
    bool to_delete;
} convertOption = {};

bool parse_files(const std::string &directory,
                 const struct convertOptions &opt_arg)
{
    namespace fs = std::experimental::filesystem;

    using std::cout;
    using std::endl;

    const std::string currentFilePath = fs::absolute(directory).string();
    const fs::path video_path = fs::path(currentFilePath);

    if (!fs::exists(video_path))
    {
        std::cerr << "Path: " << currentFilePath << " does not exist" << endl;

        return false;
    }
    else if (fs::is_regular_file(video_path))
    {
        const std::string mediafile = video_path.string();

        const auto success = hevc::check_video_is_to_convert(mediafile);
        if (success)
            hevc::process_file(mediafile, true);
    }
    else
    {
        try
        {
            const unsigned int nr_of_conversions = opt_arg.nr_of_parallel_conversions;
            static unsigned int number_of_files = opt_arg.nr_of_parallel_conversions;
            std::vector<std::string> files = {};

            for (auto &filen : fs::recursive_directory_iterator(video_path))
            {
                const std::string mediafile = filen.path().string();

                if (number_of_files > 0)
                {
                    std::cout << mediafile << std::endl;
                    const auto success = hevc::check_video_is_to_convert(mediafile);
                    if (success)
                    {
                        files.push_back(mediafile);
                        number_of_files--;
                    }
                }
                else
                {
                    std::cout << "Nr of files to convert: " << files.size() << std::endl;

                    std::for_each(std::execution::par_unseq, files.begin(),
                            files.end(), [nr_of_conversions](std::string input_file)
                            {
                            if (nr_of_conversions == 1)
                            hevc::process_file(input_file, true);
                            else
                            hevc::process_file(input_file, false);
                            });

                     number_of_files = nr_of_conversions;

                     files.clear();
                }
            }

            if(files.size() > 0) {
                std::cout << "Nr of files to convert: " << files.size() << std::endl;
                
                std::for_each(std::execution::par_unseq, files.begin(),
                        files.end(), [nr_of_conversions](std::string input_file)
                        {
                        if (nr_of_conversions == 1)
                        hevc::process_file(input_file, true);
                        else
                        hevc::process_file(input_file, false);
                        });


            }
        }
        catch (fs::filesystem_error &e)
        {
            cout << "wrong path:" << e.what() << endl;
        }
    }

    return true;
}

int main(int argc, const char **argv)
{

    bool show_help = false;
    std::string directory;

    auto parser =
        lyra::help(show_help).description(
            "This is an utility for converting video files to x265 format \nwithin "
            "a directory.") |
        lyra::opt(directory, "dir")["--directory"]["-d"]
        ("specify directory or file to use for convertion (REQUIRED).")
            .required() |
        lyra::opt(convertOption.to_delete)["--delete"]["-d"]("delete original \
        file upon successful convertion.") |
        lyra::opt(convertOption.nr_of_parallel_conversions, "nr_of_parallel_conversions")["--nr_of_parallel_conversions"]["-n"]
        ("number of parallel execution actions. (efficient usage of the existing processor cores)");

    // Parse the program arguments:
    auto result = parser.parse({argc, argv});

    // Check that the arguments where valid:
    if (!result)
    {
        std::cerr << "Error in command line: " << result.message()
                  << std::endl;
        std::cerr << parser << std::endl;

        std::exit(1);
    }
    else if (show_help)
    {

        std::cout << parser << std::endl;
    }
    else
    {
        std::cerr << "Nr of Parralel conversions: " << convertOption.nr_of_parallel_conversions << std::endl;
        parse_files(directory, convertOption);
    }
}
