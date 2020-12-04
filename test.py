import math
import matplotlib


def convertToRGB(hue, saturation, intensity):
    hue_conv = math.pi * float(hue) / 180
    sat_conv = float(saturation) / 100
    int_conv = float(intensity) / 100
    if hue_conv < 2.09439:
        r = 255 * int_conv / 3 * (1 + sat_conv * math.cos(hue_conv) / math.cos(1.047196667 - hue_conv)) + 0.5
        g = 255 * int_conv / 3 * (1 + sat_conv * (1 - math.cos(hue_conv) / math.cos(1.047196667 - hue_conv))) + 0.5
        b = 255 * int_conv / 3 * (1 - sat_conv) + 0.5
    elif hue_conv < 4.188787:
        hue_conv -= 2.09439
        g = 255 * int_conv / 3 * (1 + sat_conv * math.cos(hue_conv) / math.cos(1.047196667 - hue_conv)) + 0.5
        b = 255 * int_conv / 3 * (1 + sat_conv * (1 - math.cos(hue_conv) / math.cos(1.047196667 - hue_conv))) + 0.5
        r = 255 * int_conv / 3 * (1 - sat_conv) + 0.5
    else:
        hue_conv -= 4.188787
        b = 255 * int_conv / 3 * (1 + sat_conv * math.cos(hue_conv) / math.cos(1.047196667 - hue_conv)) + 0.5
        r = 255 * int_conv / 3 * (1 + sat_conv * (1 - math.cos(hue_conv) / math.cos(1.047196667 - hue_conv))) + 0.5
        g = 255 * int_conv / 3 * (1 - sat_conv) + 0.5
    return int(r), int(g), int(b)


def convertToHSI(red, green, blue):
    # float w, i;
    red_conv = red / 255
    green_conv = green / 255
    blue_conv = blue / 255
    i = (red_conv + green_conv + blue_conv) / 3
    if i <= 0:
        i = 1.e-6
    red_conv /= i
    green_conv /= i
    blue_conv /= i

    if red_conv == green_conv and green_conv == blue_conv:
        return 0, 0, int(i * 100)
    else:
        w = 0.5 * (red_conv - green_conv + red_conv - blue_conv) / math.sqrt((red_conv - green_conv) * (red_conv - green_conv) + (red_conv + blue_conv) * (green_conv - blue_conv))
        if w > 1:
            w = 1
        elif w < -1:
            w = -1
        h = math.acos(w)
        if blue_conv > green_conv:
            h = 2 * math.pi - h
        if red_conv <= green_conv and red_conv <= blue_conv:
            s = 1 - 3 * red_conv
        if green_conv <= red_conv and green_conv <= blue_conv:
            s = 1 - 3 * green_conv
        if blue_conv <= red_conv and blue_conv <= green_conv:
            s = 1 - 3 * blue_conv
        return int(h * 360), int(s * 100), int(i * 100)


kelvin_table = {
    2000: (255, 138, 18),
    2100: (255, 142, 33),
    2200: (255, 147, 44),
    2300: (255, 152, 54),
    2400: (255, 157, 63),
    2500: (255, 161, 72),
    2600: (255, 165, 79),
    2700: (255, 169, 87),
    2800: (255, 173, 94),
    2900: (255, 177, 101),
    3000: (255, 180, 107),
    3100: (255, 184, 114),
    3200: (255, 187, 120),
    3300: (255, 190, 126),
    3400: (255, 193, 132),
    3500: (255, 196, 137),
    3600: (255, 199, 143),
    3700: (255, 201, 148),
    3800: (255, 204, 153),
    3900: (255, 206, 159),
    4000: (255, 209, 163),
    4100: (255, 211, 168),
    4200: (255, 213, 173),
    4300: (255, 215, 177),
    4400: (255, 217, 182),
    4500: (255, 219, 186),
    4600: (255, 221, 190),
    4700: (255, 223, 194),
    4800: (255, 225, 198),
    4900: (255, 227, 202),
    5000: (255, 228, 206),
    5100: (255, 230, 210),
    5200: (255, 232, 213),
    5300: (255, 233, 217),
    5400: (255, 235, 220),
    5500: (255, 236, 224),
    5600: (255, 238, 227),
    5700: (255, 239, 230),
    5800: (255, 240, 233),
    5900: (255, 242, 236),
    6000: (255, 243, 239),
    6100: (255, 244, 242),
    6200: (255, 245, 245),
    6300: (255, 246, 247),
    6400: (255, 248, 251),
    6500: (255, 249, 253)}

def kelvinToRGB(kelvin):
    temp = kelvin/100
    if temp <= 66:
        red = 255
        green = temp
        green = 99.4708025861 * math.log(green) - 161.1195681661
        if temp <= 19:
            blue = 0
        else:
            blue = temp-10
            blue = 138.5177312231 * math.log(blue) - 305.0447927307
    else:
        red = temp - 60
        red = 329.698727446 * math.pow(red, -0.1332047592)
        
        green = temp - 60
        green = 288.1221695283 * math.pow(green, -0.0755148492)

        blue = 255
    return red, green, blue

def rgbToHSI(r, g, b):
    r = float(r)
    g = float(g)
    b = float(b)
    h = 0
    s = 0
    i = (r + g + b) / 3.0
    
    if i == 0:
        s = 0
    else:
        m = min(r, g, b)
        s = 1 - m / i
    
    if r == b and g == b:
        h = 0
    else:
        aux = (r - g/2 - b/2) / math.sqrt(r*r + g*g + b*b - r*g - r*b - g*b)
        if g >= b:
            h = math.acos(aux) * 180 / math.pi
        else:
            h = 360 - (math.acos(aux) * 180 / math.pi)

    i = 255 * math.pow(3*i/255, 1/2.8) + 0.5
    return h, s*100, i

if __name__ == '__main__':
    for kelvin, (given_r, given_g, given_b) in kelvin_table.items():
        conv_r, conv_g, conv_b = kelvinToRGB(kelvin)
        h, s, i = rgbToHSI(conv_r, conv_g, conv_b)
        print(f"{kelvin}: ({given_r}, {given_g}, {given_b}) to ({h}, {s}, {i})")
    print(rgbToHSI(255, 0, 1))
    # for sat in [0, 50, 100]:
    #     for i in range(0, 101):
    #         r, g, b = convertToRGB(0, sat, i)
    #         h, s, intens = convertToHSI(r, g, b)
    #         equals = h == 0 and s == sat and intens == i
    #         print("HSI({}, {}, {}) = RGB({}, {}, {}) = HSI({}, {}, {}) == {}".format(0, sat, i, r, g, b, h, s, intens, equals))
