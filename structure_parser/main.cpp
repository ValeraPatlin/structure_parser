#include <iostream>
#include <vector>
#include <algorithm>

struct S
{
	uint8_t prefix[3] = { 'A', 'B', 'C' };
	uint8_t size;
	std::vector<uint8_t> data;
	uint8_t crc;

    void print() const
    {
        std::cout << "Prefix: ";
        for (const auto& elem : prefix)
        {
            std::cout << elem << " ";
        }
        std::cout << "\n";
        std::cout << "Size: " << static_cast<int>(size) << "\n";
        std::cout << "Data: ";
        for (const auto& elem : data)
        {
            std::cout << elem << " ";
        }
        std::cout << "\n";
        std::cout << "CRC: " << static_cast<int>(crc) << "\n";
    }
};


S parserOne(const std::vector<uint8_t>& vec);

S parserTwo(const std::vector<uint8_t>& vec);


int main()
{
    std::vector<uint8_t> dataBuffer =
    {
        0x01, 0x02, 'A', 'C', 0x03, 0x04, 'A',
        'A', 'B', 'C',
        0x05,
        0x15, 0x16, 0x17, 0x18, 0x19,
        0xA1
    };

    std::cout << "Parsing buffer one:\n";
    S dataOne = parserOne(dataBuffer);
    dataOne.print();

    std::cout << "\nParsing buffer two:\n";
    S dataTwo = parserTwo(dataBuffer);
    dataTwo.print();

	return 0;
}

S parserOne(const std::vector<uint8_t>& vec)
{
    S result;

    for (int i = 0; i < vec.size(); ++i)
    {
        if (i + 2 >= vec.size())
        {
            break;
        }

        if (vec[i] == 'A' && vec[i + 1] == 'B' && vec[i + 2] == 'C')
        {
            size_t currentIndex = i + 3;

            if (currentIndex >= vec.size())
            {
                break;
            }

            uint8_t dataSize = vec[currentIndex];
            ++currentIndex;

            if (currentIndex + dataSize + 1 > vec.size())
            {
                break;
            }

            result.size = dataSize;
            result.data.reserve(dataSize);

            for (size_t j = 0; j < dataSize; ++j)
            {
                result.data.push_back(vec[currentIndex++]);
            }

            result.crc = vec[currentIndex];
            return result;
        }
    }
    return result;
}


S parserTwo(const std::vector<uint8_t>& vec)
{
    S result;
    const uint8_t prefixToFind[3] = { 'A', 'B', 'C' };

    auto itPrefixStart = std::search(vec.begin(), vec.end(),
        std::begin(prefixToFind), std::end(prefixToFind));

    if (itPrefixStart == vec.end())
    {
        return result;
    }

    auto itPrefixEnd = std::next(itPrefixStart, 3);

    if (itPrefixEnd == vec.end())
    {
        return result;
    }

    uint8_t dataSize = *itPrefixEnd;
    auto itDataStart = std::next(itPrefixEnd, 1);

    size_t requiredSize = static_cast<size_t>(dataSize) + 1;

    if (std::distance(itDataStart, vec.end()) < requiredSize)
    {
        return result;
    }

    std::copy(itPrefixStart, itPrefixEnd, result.prefix);
    result.size = dataSize;

    auto it_data_end = std::next(itDataStart, dataSize);
    result.data.assign(itDataStart, it_data_end);
    result.crc = *it_data_end;
    return result;
}