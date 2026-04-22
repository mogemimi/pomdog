// Copyright mogemimi. Distributed under the MIT license.

package ico

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"image"
	"image/png"
	"io"

	"golang.org/x/image/draw"
)

type IconDirectoryHeader struct {
	Reserved   uint16
	ImageType  uint16
	ImageCount uint16
}

type IconDirectoryEntry struct {
	ImageWidth   uint8
	ImageHeight  uint8
	ColorCount   uint8
	Reserved     uint8
	ColorPlanes  uint16
	BitsPerPixel uint16
	SizeInBytes  uint32
	ImageOffset  uint32
}

func Encode(w io.Writer, imgSrc image.Image) error {
	const MaxImageWidth = 256
	const MaxImageHeight = 256

	bounds := image.Rect(0, 0, imgSrc.Bounds().Dx(), imgSrc.Bounds().Dy())
	if bounds.Dx() > MaxImageWidth || bounds.Dy() > MaxImageHeight {
		bounds = image.Rect(0, 0, MaxImageWidth, MaxImageHeight)
	}

	imgResized := image.NewRGBA(bounds)

	if bounds.Dx() != imgSrc.Bounds().Dx() || bounds.Dy() != imgSrc.Bounds().Dy() {
		// NOTE: Resize
		draw.CatmullRom.Scale(imgResized, imgResized.Bounds(), imgSrc, imgSrc.Bounds(), draw.Over, nil)
	} else {
		// NOTE: Copy
		draw.NearestNeighbor.Scale(imgResized, imgResized.Bounds(), imgSrc, imgSrc.Bounds(), draw.Over, nil)
	}

	pngEncoder := png.Encoder{
		CompressionLevel: png.BestCompression,
	}

	var pngBuf bytes.Buffer
	if err := pngEncoder.Encode(&pngBuf, imgResized); err != nil {
		return err
	}

	const HeaderBytes = 6
	const DirectoryEntryBytes = 16

	header := IconDirectoryHeader{
		Reserved:   0,
		ImageType:  1,
		ImageCount: 1,
	}
	entry := IconDirectoryEntry{
		ImageWidth:   uint8(imgResized.Bounds().Dx()),
		ImageHeight:  uint8(imgResized.Bounds().Dy()),
		ColorCount:   0,
		Reserved:     0,
		ColorPlanes:  1,
		BitsPerPixel: 32,
		SizeInBytes:  uint32(len(pngBuf.Bytes())),
		ImageOffset:  uint32(HeaderBytes + DirectoryEntryBytes),
	}

	// NOTE: The ImageWidth and ImageHeight are the dimensions of the image.
	//       Originally, the supported range was 1 through 255, but starting in Windows 95,
	//       the value 0 is accepted as representing a width or height of 256.
	//       https://devblogs.microsoft.com/oldnewthing/20101018-00/?p=12513
	if imgResized.Bounds().Dx() == MaxImageWidth {
		entry.ImageWidth = 0
	}
	if imgResized.Bounds().Dy() == MaxImageHeight {
		entry.ImageHeight = 0
	}

	if err := binary.Write(w, binary.LittleEndian, header); err != nil {
		return fmt.Errorf("binary.Write() failed: %w", err)
	}

	if err := binary.Write(w, binary.LittleEndian, entry); err != nil {
		return fmt.Errorf("binary.Write() failed: %w", err)
	}

	if _, err := w.Write(pngBuf.Bytes()); err != nil {
		return fmt.Errorf("w.Write() failed: %w", err)
	}

	return nil
}

// EncodeMulti writes multiple sizes of imgSrc into a single ICO file.
// Each value in sizes specifies a square icon edge length in pixels.
// Values larger than 256 are capped to 256 per the ICO specification.
// Images are resampled with CatmullRom.
func EncodeMulti(w io.Writer, imgSrc image.Image, sizes []int) error {
	const MaxSize = 256
	const HeaderBytes = 6
	const DirectoryEntryBytes = 16

	pngEncoder := png.Encoder{
		CompressionLevel: png.BestCompression,
	}

	type encodedImage struct {
		width  int
		height int
		data   []byte
	}

	images := make([]encodedImage, 0, len(sizes))
	for _, size := range sizes {
		s := size
		if s > MaxSize {
			s = MaxSize
		}

		imgResized := image.NewRGBA(image.Rect(0, 0, s, s))
		draw.CatmullRom.Scale(imgResized, imgResized.Bounds(), imgSrc, imgSrc.Bounds(), draw.Over, nil)

		var buf bytes.Buffer
		if err := pngEncoder.Encode(&buf, imgResized); err != nil {
			return fmt.Errorf("png.Encode() failed for size %d: %w", size, err)
		}
		images = append(images, encodedImage{width: s, height: s, data: buf.Bytes()})
	}

	header := IconDirectoryHeader{
		Reserved:   0,
		ImageType:  1,
		ImageCount: uint16(len(images)),
	}
	if err := binary.Write(w, binary.LittleEndian, header); err != nil {
		return fmt.Errorf("binary.Write() failed: %w", err)
	}

	// NOTE: The first image data begins right after the header and all directory entries.
	dataOffset := uint32(HeaderBytes + DirectoryEntryBytes*len(images))

	for _, img := range images {
		imgWidth := uint8(img.width)
		imgHeight := uint8(img.height)
		// NOTE: Value 0 represents 256 per the ICO spec (Windows 95+).
		//       https://devblogs.microsoft.com/oldnewthing/20101018-00/?p=12513
		if img.width == MaxSize {
			imgWidth = 0
		}
		if img.height == MaxSize {
			imgHeight = 0
		}
		entry := IconDirectoryEntry{
			ImageWidth:   imgWidth,
			ImageHeight:  imgHeight,
			ColorCount:   0,
			Reserved:     0,
			ColorPlanes:  1,
			BitsPerPixel: 32,
			SizeInBytes:  uint32(len(img.data)),
			ImageOffset:  dataOffset,
		}
		if err := binary.Write(w, binary.LittleEndian, entry); err != nil {
			return fmt.Errorf("binary.Write() failed: %w", err)
		}
		dataOffset += uint32(len(img.data))
	}

	for _, img := range images {
		if _, err := w.Write(img.data); err != nil {
			return fmt.Errorf("w.Write() failed: %w", err)
		}
	}

	return nil
}
