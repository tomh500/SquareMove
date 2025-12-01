from PIL import Image

# 打开图片
img = Image.open("logo.png").convert("RGBA")

datas = img.getdata()

newData = []
for item in datas:
    # item = (R, G, B, A)
    if item[0] > 240 and item[1] > 240 and item[2] > 240:  # 接近白色
        newData.append((255, 255, 255, 0))  # alpha=0
    else:
        newData.append(item)

img.putdata(newData)
img.save("output.png", "PNG")
