import math


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


if __name__ == '__main__':
    for sat in [0, 50, 100]:
        for i in range(0, 101):
            r, g, b = convertToRGB(0, sat, i)
            h, s, intens = convertToHSI(r, g, b)
            equals = h == 0 and s == sat and intens == i
            print("HSI({}, {}, {}) = RGB({}, {}, {}) = HSI({}, {}, {}) == {}".format(0, sat, i, r, g, b, h, s, intens, equals))
