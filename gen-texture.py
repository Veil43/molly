width = 255
height = 255
max_color_value = 255

def write_p6_ppm(filename, width, height, color):
    with open(filename, 'wb') as f:
        f.write(f"P6\n{width} {height}\n255\n".encode())

        pixel = bytes(color)
        f.write(pixel * width * height)

base = "assets/textures/"
write_p6_ppm(base + "whiteP6.ppm", 255, 255, (255, 255, 255))
write_p6_ppm(base + "blackP6.ppm", 255, 255, (0, 0, 0))
write_p6_ppm(base + "errorP6.ppm", 255, 255, (255, 0, 255))
