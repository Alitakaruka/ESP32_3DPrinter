img_path = "medusa_128x64_bw.png"
img = Image.open(img_path).convert("1")  # Convert to monochrome (1-bit)

# Ensure size is 128x64
img = img.resize((128, 64))

# Convert to numpy array
pixels = np.array(img, dtype=np.uint8)

# For SSD1306 we usually pack 8 vertical pixels into 1 byte (page addressing)
height, width = pixels.shape
pages = height // 8

# Create buffer (8 pages × 128 columns)
ssd1306_buf = np.zeros((pages, width), dtype=np.uint8)

for page in range(pages):
    for x in range(width):
        byte = 0
        for bit in range(8):
            pixel = pixels[page*8 + bit, x]
            if pixel == 0:  # In PIL "1" mode, white=255, black=0
                byte |= (1 << bit)
        ssd1306_buf[page, x] = byte

ssd1306_buf.shape