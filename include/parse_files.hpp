#ifndef PARSE_VID_FILES
#define PARSE_VID_FILES

/*
 * Maxime Moge dda119141@gmail.com 05.07.2021
 *
*/

#include <iostream>
#include <future>
#include <string>
#include <experimental/filesystem>
#include <boost/process.hpp>

namespace hevc
{

    std::string str_tolower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), 
                [](unsigned char c){ return std::tolower(c); } 
                );
        return s;
    }

    bool replace_string(std::string& text, const std::string key, const std::string value)
    {
        std::string::size_type begin = text.find(key);
        if(begin == std::string::npos) //return if there's no key in text
            return false;

        std::string::size_type pos = 0;
        for(; begin != std::string::npos; begin = text.find(key, pos))
        {
            text.replace(begin, key.size(), value);
            pos = begin + key.size();
        }

        return true;
    }

    std::pair<bool, std::vector<std::string>> executeCommand(const std::string& command, bool print_output = false)
    {
        using namespace boost::process;

        ipstream pipe_stream;
        child c(command.c_str(), std_out > pipe_stream);

        std::string line;
        std::vector<std::string> command_output;

        while (pipe_stream && std::getline(pipe_stream, line) && !line.empty()){
            command_output.push_back(line);

            if(print_output) {
                std::cout << line.c_str() << std::endl;
            }
        }
        c.wait();

        if(c.exit_code() == 0)
            return { true, command_output };
        else
            return { false, command_output };
    }

    auto format_ending_to_hevc(const std::string input_file)
    {
        namespace fs = std::experimental::filesystem;
        
        fs::path file_path = { input_file };

        auto ending = file_path.extension();

        const fs::path new_ending = { std::string(".x265") + ending.string() };

        return file_path.replace_extension(new_ending);
    }

    bool convert_to_x265(const std::string& input_file, bool print_output = false)
    {   
        auto check_f = [&input_file, print_output]()
        {
            std::string output_file = input_file;

            if (!replace_string(output_file, "264", "265")){
                //output_file = input_file + std::string(".x265");
                output_file = format_ending_to_hevc(input_file).string();
            }

            const std::string command = std::string("ffmpeg -i \"") + input_file 
                + std::string("\" -c:v libx265 -map 0:v -map 0:a? \"") + output_file
                + std::string("\"");

            if(print_output)
                std::cout << command << std::endl;

            auto ret = executeCommand(command, print_output);

            return (std::get<0>(ret));
        };

        return check_f();
    }

    bool rename_file(const std::string& input_file)
    {
        const std::string outp = input_file + std::string{ ".done" };

        return std::rename(input_file.c_str(), outp.c_str());
    }

    bool element_exist(const std::vector<std::string> cont, const std::string& elt)
    {
        return std::any_of(cont.cbegin(),
                cont.cend(), [&elt](const std::string& cont_elt)
                { return str_tolower(cont_elt) == elt; }
                );
    }

    bool check_video_file_is_hevc(const std::string& input_file)
    {   
        auto check_f = [&input_file]() -> bool
        {
            const std::string command = std::string("ffprobe -v error -select_streams v -of \
                    default=noprint_wrappers=1:nokey=1 \
                    -show_entries stream=codec_name \"") + input_file + std::string("\"");

            const auto ret = executeCommand(command, true);

            auto& command_output = std::get<1>(ret);

            if(element_exist(command_output, std::string("hevc")))
                return true;
            else if(element_exist(command_output, std::string("h265")))
                return true;
            else
                return false;
        };

        return check_f();
    }

    bool check_video_is_to_convert(const std::string& input_file)
    {   
        auto check_f = [&input_file]() -> bool
        {
            const std::string command = std::string("ffprobe -v error -select_streams v -of \
                    default=noprint_wrappers=1:nokey=1 \
                    -show_entries stream=codec_type \"") + input_file + std::string("\"");

            const auto ret = executeCommand(command, true);

            auto& command_output = std::get<1>(ret);

            if(check_video_file_is_hevc(input_file)) // it is already a hevc video file
                return false;
            else if(input_file.rfind(".done") != std::string::npos)
//            else if(input_file.rfind(".done") == (input_file.size() - std::string(".done").size()) )
                return false; // element already converted
            else
                return element_exist(command_output, std::string("video"));
        };

        return check_f();
    }

    bool process_file(const std::string& input_file, bool print_output = false)
    {
        std::cout << "Conversion Start: " << input_file << std::endl;

        auto process_video_file = [&input_file, print_output]()
        {
            const auto success = convert_to_x265(input_file, print_output);

            std::cout << "Conversion End: " << input_file << std::endl;

            if(success){
                return rename_file(input_file);
            }
            else {
                return success;
            }
        };

        return process_video_file();

    }

} // hevc namespace
#endif  // PARSE_VID_FILES
