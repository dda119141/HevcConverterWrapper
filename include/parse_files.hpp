#ifndef PARSE_VID_FILES
#define PARSE_VID_FILES

/*
 * Maxime Moge dda119141@gmail.com 05.07.2021
 *
*/

#include <iostream>
#include <future>
#include <string>
#include <boost/process.hpp>

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

std::pair<bool, std::string> executeCommand(const std::string& command, bool print_output = false)
{
     using namespace boost::process;

        ipstream pipe_stream;
        child c(command.c_str(), std_out > pipe_stream);

        std::string line;
        std::string command_output;

        while (pipe_stream && std::getline(pipe_stream, line) && !line.empty()){
            command_output = line;

            if(print_output) {
                 std::cout << line.c_str() << std::endl;
            }
        }
        c.wait();

        return { c.exit_code(), command_output };

}

bool convert_to_x265(const std::string& input_file, bool print_output = false)
{   
    auto check_f = [&input_file, print_output]()
    {
        std::string output_file = input_file;

        if (!replace_string(output_file, "264", "265")){
            output_file = input_file + std::string(".x265");
        }

        const std::string command = std::string("ffmpeg -i '") + input_file 
               + std::string("' -c:v libx265 -map 0:v -map 0:a? '") + output_file
               + std::string("'");

        if(print_output)
            std::cout << command << std::endl;

        auto ret = executeCommand(command, print_output);

        return (std::get<0>(ret));
    };

    return check_f();
}

bool rename_file(const std::string& input_file)
{
    const std::string outp = input_file + std::string(".done");

    return std::rename(input_file.c_str(), outp.c_str());
}

bool check_video_file(const std::string& input_file)
{   
    auto check_f = [&input_file]() -> bool
    {
        const std::string command = std::string("ffprobe -v error -select_streams v -of \
                default=noprint_wrappers=1:nokey=1 \
                -show_entries stream=codec_type ") + input_file;

        const auto ret = executeCommand(command, true);

        auto& command_output = std::get<1>(ret);

        return (command_output == std::string("video"));
    };

    return check_f();
}

bool process_file(const std::string& input_file, bool print_output = false)
{
    std::cout << "Conversion Start: " << input_file.c_str() << std::endl;

    auto process_video_file = [&input_file, print_output]()
    {
        const auto success = convert_to_x265(input_file, print_output);

        std::cout << "Conversion End: " << input_file.c_str() << std::endl;
        
        if(success){
            return rename_file(input_file);
        }
        else {
            return success;
        }
    };

    return process_video_file();

}


#endif  // PARSE_VID_FILES
