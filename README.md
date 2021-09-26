# Qiewer
The simplest image viewer for my use.　　


# Features
- Single process application.
   - All images will be displayed in their own tab.
   - <span style="color: gray;">(and not support multiple windows)</span>
- Small binary size (smaller than image file).
- Faster than Windows10 PhotoViewer.
- Available image formats (supported by Qt)
   - .png
   - .jpg
   - .bmp
   - .gif (GIF Animation is not supported yet)
   - .svg, .svgz


# Requirement
- Qt5 runtime libraries
  - download here:
    https://www.qt.io/download-open-source


# Update plan (no more than a plan)
- Add tool bar and following icons (partially work in progress)
   - Reload
   - Setting (open in new dialog)
      - Language
      - Process lifespan (Keep alive / Exit when all images are closed)
      - Default image size (Fit to window / Original size)
      - About Qt
      - About Qiewer
- GIF Animation Support (far from work in progress)
   - Video Support, if possible

