#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

int main()
{
    const std::size_t fixed_buffer_size = 1ULL * 1024 * 1024;

    std::string input_file = ".\\data.txt";
    std::string output_file = ".\\data-copy.txt";

    std::ifstream infile(input_file, std::ios::binary);
    std::ofstream outfile(output_file, std::ios::binary);

    if (!infile || !outfile)
    {
        std::cerr << "Could not open file\n";
        return 1;
    }

    std::vector<char> buffer(fixed_buffer_size);

    auto start = std::chrono::steady_clock::now();

    while (true)
    {
        infile.read(buffer.data(), fixed_buffer_size);

        std::streamsize actual_bytes = infile.gcount();

        if (actual_bytes == 0)
            break;

        outfile.write(buffer.data(), actual_bytes);

        auto now = std::chrono::steady_clock::now();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

        std::cout << "\rTime: " << seconds << " seconds" << std::flush;
    }

    auto end = std::chrono::steady_clock::now();

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    std::cout << "\nFinished!\n";
    std::cout << "Total time: " << seconds << " seconds\n";

    return 0;
}