import os.path
import PIL.Image
import PIL.ImageDraw
import PIL.ImageFont
import qrcode
import rstr
import argparse

script_dir = os.path.dirname(os.path.realpath(__file__))

BASE36 = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'

PAYLOAD_VERSION = 0
PAYLOAD_FLAGS = 2  # 2=IP, 4=BLE, 8=IP_WAC

def gen_homekit_setup_uri(category, password, setup_id, version=0, reserved=0, flags=2):
    payload = 0
    payload |= (version & 0x7)

    payload <<= 4
    payload |=(reserved & 0xf)  # reserved bits

    payload <<= 8
    payload |= (category & 0xff)

    payload <<= 4
    payload |= (flags & 0xf)

    payload <<= 27
    payload |= (int(password.replace('-', '')) & 0x7fffffff)

    encodedPayload = ''
    for _ in range(9):
        encodedPayload += BASE36[payload % 36]
        payload //= 36

    return 'X-HM://%s%s' % (''.join(reversed(encodedPayload)), setup_id)


def gen_homekit_qrcode(setup_uri, password):
    code = qrcode.QRCode(version=2, border=0, box_size=12,
                         error_correction=qrcode.constants.ERROR_CORRECT_Q)
    code.add_data(setup_uri)

    # open template
    img = PIL.Image.open(os.path.join(script_dir, 'qrcode-model.png'))
    # add QR code to it
    img.paste(code.make_image().get_image(), (50, 180))

    # add password digits
    setup_code = password.replace('-', '')

    font = PIL.ImageFont.truetype(os.path.join(script_dir, 'Arial Bold.ttf'), 56)
    draw = PIL.ImageDraw.Draw(img)

    for i in range(4):
        draw.text((170 + i*50, 35), setup_code[i], font=font, fill=(0, 0, 0))
        draw.text((170 + i*50, 95), setup_code[i+4], font=font, fill=(0, 0, 0))

    return img


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--password", type=str, default=None)
    parser.add_argument("--setupid", type=str, default=None)
    parser.add_argument("--category", type=int, default=5)
    parser.add_argument("--filename", type=str, default="qrcode")
    args = parser.parse_args()

    if args.password is None:
        password = rstr.xeger("\d{3}-\d{2}-\d{3}")
        setup_id = rstr.xeger("[0-9A-Z]{4}")
    else:
        password = args.password
        if args.setupid is not None:
            setup_id = args.setupid
        else:
            setup_id = ""
    print(f"-D HKPASSWORD=\\\"{password}\\\"")
    print(f"-D HKSETUPID=\\\"{setup_id}\\\"")
    setupURI = gen_homekit_setup_uri(args.category, password, setup_id)
    qrcodeImage = gen_homekit_qrcode(setupURI, password)
    qrcodeImage.save(f"{args.filename}.png")


if __name__ == '__main__':
    main()