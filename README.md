# Qiewer
The simplest image viewer for my use.　　


# Features
- Single process application.
   - All images will be displayed in their own tab.
   - <span style="color: gray;">(and not support multiple windows)</span>
- Small binary size (smaller than image file, excluding QuickTime runtime).
- Faster than Windows10 PhotoViewer.
- Available image formats (supported by Qt)
   - .png
   - .jpg
   - .bmp
   - .gif (GIF Animation is not supported)
   - .svg


# Requirement
- QuickTime5 runtime libraries


# Update plan (no more than a plan)
- Version compatibility
   - ~~Add version tag to .qiewerconfig~~ Done
- Add tool bar and following icons (not work in progress)
   - ~~Open File (load image from files electer)~~ Done
   - Reload
   - ~~Fit to window~~ Done
   - ~~View in original size~~ Done
   - Setting (open in new dialog)
      - Window mode (Maximized / Normal)
	  - Language
	  - Drag & drop limitation (8 files by  default)
      - Process lifespan (Keep alive / Exit when all images are closed)
      - Default image size (Fit to window / Original size)
      - Maximum & Minimum scale (0.1 to 10.0, by default)
      - Scale change rate (x10^0.1 per 15° wheel rotation, by default)
      - About Qt
      - About Qiewer
   - ~~Drag & Drop Support~~ Done
   - GIF Animation Support (far from work in progress)
      - Video Support, if possible

