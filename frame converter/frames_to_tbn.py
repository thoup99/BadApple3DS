from PIL import Image

start_frame = 0
frames = 6570

threshold = 125

data = []
for z in range(frames):
    if z % 100 == 0:
        print(f"Encoding frame {z}")
    im = Image.open(f"./frames2/frame_{start_frame + z}.jpg")
    pix = im.load()
    width, height = im.size

    if z == 444: threshold = 50
    elif z == 817: threshold = 125

    #row = []
    #First 50 bytes are for storing the first color of each row
    col = [0x00] * 50
    for x in range(width):
        last = 0
        in_a_row = 1
        count = 0
        for y in range(height):
            r, g, b = pix[x, y]
            val = 0 if r < threshold else 1

            #If the current pixel is the first in its col
            if y == 0:
                color_byte = x // 8 #Which color_byte to access (This is one of the first 50 bytes in the col variable)
                col[color_byte] <<= 1 #Shift the bits over one to the left to "lock" what was the rightmost bit in place
                col[color_byte] |= val # bitwise or the current value to set the rightmost bit of the byte

                last = val
                continue

            if last == val:
                in_a_row += 1
                

            if last != val or y == height - 1:
                col.append(in_a_row)
                count += in_a_row

                if y == height - 1 and last != val:
                    col.append(1)
                    count += 1

                last = val
                in_a_row = 1

        if count != height:
            print(f"Count is only {count}")

    for element in col:
        data.append(element)

with open("bad_apple.tbn", "wb") as file:
    file.write(bytearray(data))
