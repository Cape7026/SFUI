#include <iostream>
#include <fstream>
#include <vector>

#include <cstdint>
#include <algorithm>

int main()
{
    const std::uint64_t file_size = 10ULL * 1024 * 1024 * 1024;
    const std::size_t buffer_size = 1024 * 1024;

    std::ofstream outfile("data.txt", std::ios::binary);

    if (!outfile)
    {
        std::cerr << "Could not create file\n";
        return 1;
    }

    std::vector<char> buffer(buffer_size, 'A');

    std::uint64_t written = 0;

    while (written < file_size)
    {
        std::uint64_t remaining = file_size - written;
        std::size_t bytes_to_write = static_cast<std::size_t>(
            std::min<std::uint64_t>(remaining, buffer_size)
        );

        outfile.write(buffer.data(), bytes_to_write);
        written += bytes_to_write;
    }

    outfile.close();

    std::cout << "Created 10 GiB file successfully.\n";

    return 0;
}