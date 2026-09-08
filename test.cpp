#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iomanip>

void progress_bar(double &progress_in_percentage, float &current_progress)
{

    float progress_in_percentage_float = float(progress_in_percentage);
    current_progress = progress_in_percentage_float;

    // std::cout << "Progress         : " << progress_in_percentage << " %" << "\n";
}

int main()
{

    const int fixed_buffer_size = (10 * 1024 * 1024);

    std::string input_file = ".\\data.txt";
    std::string output_file = ".\\data-copy.txt";

    uintmax_t src_file_size = std::filesystem::file_size(input_file);
    std::cout << "Size of source file " << input_file << " : " << src_file_size << " bytes" << "\n";

    std::ifstream infile;
    std::ofstream outfile;

    infile.open(input_file, std::ifstream::binary);
    outfile.open(output_file, std::ofstream::binary);

    if (!infile || !outfile)
    {
        std::cerr << "Could not open file\n";
        return 1;
    }

    std::vector<char> buffer(fixed_buffer_size);

    uintmax_t bytes_copied = 0;
    float current_progress = 0;
    float next_milestone = 0;

    while (true)
    {
        infile.read(buffer.data(), fixed_buffer_size);
        std::streamsize actual_bytes_read = infile.gcount();
        if (actual_bytes_read == 0)
        {
            break;
        }
        outfile.write(buffer.data(), actual_bytes_read);

        bytes_copied += actual_bytes_read;
        double progress_in_percentage = (static_cast<double>(bytes_copied) / src_file_size) * 100;

        progress_bar(progress_in_percentage, current_progress);

        if (current_progress >= next_milestone)
        {
            std::cout << "\rProgress         : " << std::fixed << std::setprecision(1) << current_progress << " %";
            next_milestone = current_progress + 0.1f;
        }

        if (bytes_copied == src_file_size)
        {
            std::cout << "\rProgress         : 100.0 %";
        }
    }
    infile.close();
    outfile.close();

    return 0;
}