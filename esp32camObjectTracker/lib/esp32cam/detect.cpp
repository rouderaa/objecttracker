#include <stdint.h>
#include <Arduino.h>

void displayBMPHeader(const uint8_t *bmpBuffer, size_t bufferLength)
{
    // Check if buffer is large enough to contain a BMP header
    if (bufferLength < 54)
    {
        Serial.println(F("Error: Buffer too small to contain a valid BMP header"));
        return;
    }

    // BMP File Header - 14 bytes
    Serial.println(F("\n--- BMP FILE HEADER ---"));

    // Signature - 2 bytes
    char signature[3] = {(char)bmpBuffer[0], (char)bmpBuffer[1], '\0'};
    Serial.print(F("Signature: "));
    Serial.println(signature);

    // File Size - 4 bytes (little-endian)
    uint32_t fileSize = bmpBuffer[2] | (bmpBuffer[3] << 8) | (bmpBuffer[4] << 16) | (bmpBuffer[5] << 24);
    Serial.print(F("File Size: "));
    Serial.print(fileSize);
    Serial.println(F(" bytes"));

    // Reserved - 4 bytes
    uint16_t reserved1 = bmpBuffer[6] | (bmpBuffer[7] << 8);
    uint16_t reserved2 = bmpBuffer[8] | (bmpBuffer[9] << 8);
    Serial.print(F("Reserved1: 0x"));
    Serial.println(reserved1, HEX);
    Serial.print(F("Reserved2: 0x"));
    Serial.println(reserved2, HEX);

    // Data Offset - 4 bytes
    uint32_t dataOffset = bmpBuffer[10] | (bmpBuffer[11] << 8) | (bmpBuffer[12] << 16) | (bmpBuffer[13] << 24);
    Serial.print(F("Data Offset: "));
    Serial.print(dataOffset);
    Serial.println(F(" bytes"));

    // BMP Info Header - 40 bytes
    Serial.println(F("\n--- BMP INFO HEADER ---"));

    // Header Size - 4 bytes
    uint32_t headerSize = bmpBuffer[14] | (bmpBuffer[15] << 8) | (bmpBuffer[16] << 16) | (bmpBuffer[17] << 24);
    Serial.print(F("Header Size: "));
    Serial.print(headerSize);
    Serial.println(F(" bytes"));

    // Image Width - 4 bytes
    int32_t width = bmpBuffer[18] | (bmpBuffer[19] << 8) | (bmpBuffer[20] << 16) | (bmpBuffer[21] << 24);
    Serial.print(F("Width: "));
    Serial.print(width);
    Serial.println(F(" pixels"));

    // Image Height - 4 bytes
    int32_t height = bmpBuffer[22] | (bmpBuffer[23] << 8) | (bmpBuffer[24] << 16) | (bmpBuffer[25] << 24);
    Serial.print(F("Height: "));
    Serial.print(height);
    Serial.println(F(" pixels"));

    // Planes - 2 bytes
    uint16_t planes = bmpBuffer[26] | (bmpBuffer[27] << 8);
    Serial.print(F("Planes: "));
    Serial.println(planes);

    // Bits Per Pixel - 2 bytes
    uint16_t bitsPerPixel = bmpBuffer[28] | (bmpBuffer[29] << 8);
    Serial.print(F("Bits Per Pixel: "));
    Serial.println(bitsPerPixel);

    // Compression - 4 bytes
    uint32_t compression = bmpBuffer[30] | (bmpBuffer[31] << 8) | (bmpBuffer[32] << 16) | (bmpBuffer[33] << 24);
    Serial.print(F("Compression: "));

    // Print compression method name
    switch (compression)
    {
    case 0:
        Serial.println(F("BI_RGB (uncompressed)"));
        break;
    case 1:
        Serial.println(F("BI_RLE8"));
        break;
    case 2:
        Serial.println(F("BI_RLE4"));
        break;
    case 3:
        Serial.println(F("BI_BITFIELDS"));
        break;
    case 4:
        Serial.println(F("BI_JPEG"));
        break;
    case 5:
        Serial.println(F("BI_PNG"));
        break;
    default:
        Serial.println(compression);
        break;
    }

    // Image Size - 4 bytes
    uint32_t imageSize = bmpBuffer[34] | (bmpBuffer[35] << 8) | (bmpBuffer[36] << 16) | (bmpBuffer[37] << 24);
    Serial.print(F("Image Size: "));
    Serial.print(imageSize);
    Serial.println(F(" bytes"));

    // X Pixels Per Meter - 4 bytes
    int32_t xPixelsPerMeter = bmpBuffer[38] | (bmpBuffer[39] << 8) | (bmpBuffer[40] << 16) | (bmpBuffer[41] << 24);
    Serial.print(F("X Resolution: "));
    Serial.print(xPixelsPerMeter);
    Serial.println(F(" pixels/meter"));

    // Y Pixels Per Meter - 4 bytes
    int32_t yPixelsPerMeter = bmpBuffer[42] | (bmpBuffer[43] << 8) | (bmpBuffer[44] << 16) | (bmpBuffer[45] << 24);
    Serial.print(F("Y Resolution: "));
    Serial.print(yPixelsPerMeter);
    Serial.println(F(" pixels/meter"));

    // Colors Used - 4 bytes
    uint32_t colorsUsed = bmpBuffer[46] | (bmpBuffer[47] << 8) | (bmpBuffer[48] << 16) | (bmpBuffer[49] << 24);
    Serial.print(F("Colors Used: "));
    Serial.println(colorsUsed);

    // Important Colors - 4 bytes
    uint32_t importantColors = bmpBuffer[50] | (bmpBuffer[51] << 8) | (bmpBuffer[52] << 16) | (bmpBuffer[53] << 24);
    Serial.print(F("Important Colors: "));
    Serial.println(importantColors);
}

static void debug_rgb_info(int width, int height, int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    int size = 3;
    int start_x = (width / 2) - size;
    int end_x = (width / 2) + size;
    int start_y = (height / 2) - size;
    int end_y = (height / 2) + size;
    if ((x > start_x) && (x <= end_x) && (y > start_y) && (y <= end_y))
    {
        if ((x > start_x) && (x < end_x) && (y > start_y) && (y < end_y))
        {
            Serial.printf("(%d %d %d) ", r, g, b);
        }
        if (x == end_x)
        {
            Serial.printf("\r\n");
        }
        if ((y == end_y) && (x == end_x))
        {
            Serial.printf("---------\r\n");
        }
    }
}

bool getCalibration(
    uint8_t *buf, int buf_len,
    int &red_level, int &green_level, int &blue_level)
{
    // DEBUG
    displayBMPHeader(buf, buf_len);

    // BMP file format handling
    // The BMP header is typically 54 bytes
    const int HEADER_SIZE = 54;

    if (buf_len <= HEADER_SIZE)
    {
        Serial.println("getCalibration error: 0");
        return false; // Buffer too small to contain a valid BMP
    }

    // Extract image dimensions from BMP header
    int width = *reinterpret_cast<int *>(&buf[18]);
    int height = *reinterpret_cast<int *>(&buf[22]);
    int bitsPerPixel = *reinterpret_cast<short *>(&buf[28]);

    // Ensure it's a 24-bit BMP (RGB)
    if (bitsPerPixel != 24)
    {
        Serial.printf("getCalibration error: 1, bitsPerPixel:%d\r\n", bitsPerPixel);
        return false; // Only supporting 24-bit BMPs
    }

    // Calculate row padding (BMP rows are padded to 4-byte boundaries)
    int paddedRowSize = ((width * 3 + 3) / 4) * 4;
    int dataSize = paddedRowSize * height;

    // Check if buffer is large enough
    if (buf_len < HEADER_SIZE + dataSize)
    {
        Serial.printf("getCalibration error: 2 buf_len:%d dataSize:%d\r\n", buf_len, dataSize);
        return false; // Buffer too small for the image dimensions
    }

    // BMP files store image data bottom-up by default
    // Check if height is negative, which means top-down storage
    bool isTopDown = height < 0;
    if (isTopDown)
    {
        height = -height;
    }

    // Pointer to start of pixel data
    uint8_t *pixelData = buf + HEADER_SIZE;

    int offset = 2;
    int sum_red_level = 0;
    int sum_green_level = 0;
    int sum_blue_level = 0;
    int nrOfSamples = 0;

    for (int y = (height / 2) - offset; y < (height / 2) + offset; y++)
    {
        for (int x = (width / 2) - offset; x < (width / 2) + offset; x++)
        {
            // Calculate position in BMP data
            // BMP stores colors as BGR
            int actualY = isTopDown ? y : (height - 1 - y);
            int pos = actualY * paddedRowSize + x * 3;

            sum_red_level = sum_red_level + pixelData[pos + 2];
            sum_green_level = sum_green_level + pixelData[pos + 1];
            sum_blue_level = sum_blue_level + pixelData[pos];
            nrOfSamples++;
        }
    }
    // Calculate averages
    red_level = sum_red_level / nrOfSamples;
    green_level = sum_green_level / nrOfSamples;
    blue_level = sum_blue_level / nrOfSamples;

    return true;
}

/**
 * Detects regions in a BMP image that match specific color criteria.
 * Finds a rectangle enclosing all pixels that meet the following criteria:
 * - red value must be >= the specified red minimum
 * - green value must be <= the specified green maximum
 * - blue value must be <= the specified blue maximum
 *
 * @param buf Pointer to the BMP image data
 * @param buf_len Length of the buffer in bytes
 * @param red_level Minimum red value to match
 * @param green_level Maximum green value to match
 * @param blue_level Maximum blue value to match
 * @param left Output parameter for the left coordinate of detected rectangle
 * @param top Output parameter for the top coordinate of detected rectangle
 * @param right Output parameter for the right coordinate of detected rectangle
 * @param bottom Output parameter for the bottom coordinate of detected rectangle
 * @return true if detection successful, false otherwise
 */
bool detect(
    uint8_t *buf, int buf_len,
    int red_level, int green_level, int blue_level,
    int &left, int &top, int &right, int &bottom)
{
    // BMP file format handling
    // The BMP header is typically 54 bytes
    const int HEADER_SIZE = 54;

    if (buf_len <= HEADER_SIZE)
    {
        return false; // Buffer too small to contain a valid BMP
    }

    // Extract image dimensions from BMP header
    int width = *reinterpret_cast<int *>(&buf[18]);
    int height = *reinterpret_cast<int *>(&buf[22]);
    int bitsPerPixel = *reinterpret_cast<short *>(&buf[28]);

    // Ensure it's a 24-bit BMP (RGB)
    if (bitsPerPixel != 24)
    {
        return false; // Only supporting 24-bit BMPs
    }

    // Calculate row padding (BMP rows are padded to 4-byte boundaries)
    int paddedRowSize = ((width * 3 + 3) / 4) * 4;
    int dataSize = paddedRowSize * height;

    // Check if buffer is large enough
    if (buf_len < HEADER_SIZE + dataSize)
    {
        return false; // Buffer too small for the image dimensions
    }

    // BMP files store image data bottom-up by default
    // Check if height is negative, which means top-down storage
    bool isTopDown = height < 0;
    if (isTopDown)
    {
        height = -height;
    }

    // Pointer to start of pixel data
    uint8_t *pixelData = buf + HEADER_SIZE;

    // Initialize boundaries to extremes (for min/max search)
    left = width;    // Initialize to max possible value
    top = height;    // Initialize to max possible value
    right = -1;      // Initialize to min possible value
    bottom = -1;     // Initialize to min possible value

    bool found = false;

    // Scan the entire image to find the bounding rectangle
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Calculate position in BMP data
            // BMP stores colors as BGR
            int actualY = isTopDown ? y : (height - 1 - y);
            int pos = actualY * paddedRowSize + x * 3;

            // Check if pixel meets the color criteria
            if (pixelData[pos + 2] >= red_level &&   // R component (at offset 2)
                pixelData[pos + 1] <= green_level && // G component (at offset 1)
                pixelData[pos] <= blue_level)        // B component (at offset 0)
            {
                // Update the bounding rectangle
                left = std::min(left, x);
                top = std::min(top, y);
                right = std::max(right, x);
                bottom = std::max(bottom, y);
                found = true;
            }
        }
    }

    // If no matching pixels found, return false
    if (!found)
    {
        return false;
    }

    // Boundary adjustment
    if (top < 0)
        top = 0;
    if (top >= height)
        top = height-1;
    if (bottom < 0)
        bottom = 0;
    if (bottom >= height)
        bottom = height-1;

    // Flip the y axis
    top = height-top-1;
    bottom = height-bottom-1;

    // Serial.printf("(%d, %d) (%d, %d)\r\n", left, top, right, bottom);

    return true;
}