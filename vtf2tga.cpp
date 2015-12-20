#include <iostream>
#include <VTFLib13/VTFLib.h>
#include <VTFLib13/VTFFile.h>
#include <Magick++.h>

using namespace Magick;

int main(int argc, char* argv[]) {
	// If we haven't gotten the right parameters, show usage and exit
	if (argc < 3) {
		std::cout << "VTF Converter" << std::endl;
		std::cout << "Missing required parameters:" << std::endl;
		std::cout << argv[0] << " /path/to/source.vtf /output/out.png" << std::endl;
		exit(1);
	}
	// Load parameters into variables
	std::string infile = argv[1];
	std::string outfile = argv[2];
	std::string outformat = outfile.substr(outfile.find_last_of(".") + 1);
	std::transform(outformat.begin(), outformat.end(), outformat.begin(), ::toupper);

/*
	// Check to ensure that the output format is valid, otherwise select default
	std::set<std::string> formats {"PNG", "TGA", "JPEG", "GIF"};
	if (formats.find(outformat) == formats.end()) {
		outformat = formats.begin();
	}
	std::cout << "Saving output as " << outformat << std::endl;
*/
	// Create VTF Library object
	VTFLib::CVTFFile vtf;

	// Load input file
	if (!vtf.Load(infile.c_str())) {
		std::cout << "Failed to load " << infile << std::endl;
		exit(1);
	} else {
		std::cout << "Loaded " << infile << std::endl;
	}
	// Get image size
	vlUInt width = vtf.GetWidth();
	vlUInt height = vtf.GetHeight();
	std::string img_dimensions = std::to_string(width) + "x" + std::to_string(height);
	std::cout << "Width: " << width << std::endl;
	std::cout << "Height: " << height << std::endl;
	std::cout << "Dimensions: " << img_dimensions << std::endl;

	// Determine Alpha channel of image. This is currently not used.
	VTFImageFormat srcformat = vtf.GetFormat();
	VTFImageFormat dstformat;
	if (VTFLib::CVTFFile::GetImageFormatInfo(srcformat).uiAlphaBitsPerPixel > 0) {
		dstformat = IMAGE_FORMAT_RGBA8888;
		std::cout << "Destination format: RGBA" << std::endl;
	}
	else {
		dstformat = IMAGE_FORMAT_RGB888;
		std::cout << "Destination format: RGB" << std::endl;
	}

	// Retrieve the source image. This only gets the first frame for now.
	// TODO: Iterate over frames to export frames as individual files or GIF?
	int currentFrame = 0;
	const vlByte* frame = vtf.GetData(currentFrame, 0, 0, 0);
	if (!frame) {
		std::cout << "Failed to get the image data at frame 0." << std::endl;
		exit(1);
	}

	// Create the ImageMagic image to create output
	Image image;
	image.type(::Magick::TrueColorType);
	image.modifyImage();

	// memory copy vtflib's buffer to image magick's buffer
	VTFLib::CVTFFile tmpfile;
	tmpfile.Create(vtf.GetWidth(), vtf.GetHeight());

	// TODO: Make this honor the prior determination of alpha channel
	vtf.ConvertToRGBA8888(vtf.GetData(1,1,1,0), tmpfile.GetData(1,1,1,0), vtf.GetWidth(), vtf.GetHeight(), vtf.GetFormat());

	image.read(tmpfile.GetWidth(), tmpfile.GetHeight(), "RGBA", Magick::CharPixel, tmpfile.GetData(1,1,1,0));

	std::cout << "Writing: " << argv[2] << std::endl;

	// TODO: Output in proper format. Letting ImageMagick decide for now.
//	image.magick(outformat);
	image.write(argv[2]);

	exit(0);
}
