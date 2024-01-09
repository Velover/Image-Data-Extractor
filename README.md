# Image-Extractor

[Exe](https://drive.google.com/file/d/1wl1yD7UJQ-PY-9SXtXAyPRT9hpWh3HLd/view) -- file itself

Hello, this is my project about converting images to readable strings.
One day I was curious, can I put images to Roblox without uploading them?
This project is based on the OpenCv(C++), Qoi image encoding, zLib - library and base64 library
The Encoding order is Image -> Qoi -> base64 -> zLib.Deflate -> base64
It can convert any image to a readable string with the size of a PNG
That was a breakthrough for me in data compression
Right now I am learning JPEG / MPEG encoding, probably this project in the future going to compress a video to a string.

Usage -> Open Exe -> it will create 2 folders (Images, Out)
Put Images in Images and Open Exe once again
after compressing it's going to Out with the same name but .txt
Has protection from non-images, it will not cause crush but will be ignored

Decoding Order Data -> fromBase64 -> zLib.Deflate -> fromBase64 -> Qoi -> Image

the place with editable images --you will find the usage there as well

[ZLib](https://devforum.roblox.com/t/string-compression-zlibdeflate/755687)

[Base64](https://devforum.roblox.com/t/base64-encoding-and-decoding-in-lua/1719860)

[Qoi specification](https://qoiformat.org/qoi-specification.pdf) recreated for this project

Requires OpenCV C++ [Installation Guide](https://youtu.be/lNmA9WATqQU?si=UoaANBz_N2l5X37e&t=1672)
