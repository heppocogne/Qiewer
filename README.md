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
- Qt5 runtime libraries


# Update plan (no more than a plan)
- Add tool bar and following icons (not work in progress)
   - Reload
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
- GIF Animation Support (far from work in progress)
   - Video Support, if possible

