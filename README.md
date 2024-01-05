# Image-Extractor

Hello, this is my project about converting images to readable string.
One day I was curious, can I put images to Roblox without uploading them?
This project is based on the OpenCv(C++), Qoi image encoding, zLib - library and base64 library
The Encoding order is Image -> Qoi -> base64 -> zLib.Deflate -> base64
It can convert any image to a readable string with size of PNG
That was a major breakthrough for me in data compression
Right now I learn JPEG / MPEG encoding, probably this project in the future going to compress a video to a string.

Usage -> Open Exe -> it will create 2 folders (Images, Out)
Put Images in Images and after compressing it's going to Out with the same name but .txt
Has a protection from non-images, it will not cause crush but just will be ignored

Decoding Order Data -> fromBase64 -> zLib.Deflate -> fromBase64 -> Qoi -> Image
