# Static File Serving Guide

## Overview

The OptionPricer HTTP server provides comprehensive static file serving capabilities for all web resources. This document describes how static files are served and how to add new resources.

## How It Works

### Route Configuration

The server uses a two-tier approach for serving static files:

1. **Root Path Handler** - Serves the main application page
   ```cpp
   GET / → web/index.html
   ```

2. **Generic File Handler** - Serves any file from the web directory
   ```cpp
   GET /<filename> → web/<filename>
   ```

### MIME Type Detection

The server automatically detects the correct MIME type based on file extensions:

| Extension | MIME Type | Description |
|-----------|-----------|-------------|
| `.html`, `.htm` | `text/html` | HTML documents |
| `.css` | `text/css` | Stylesheets |
| `.js` | `application/javascript` | JavaScript files |
| `.json` | `application/json` | JSON data |
| `.png` | `image/png` | PNG images |
| `.jpg`, `.jpeg` | `image/jpeg` | JPEG images |
| `.gif` | `image/gif` | GIF images |
| `.svg` | `image/svg+xml` | SVG images |
| `.ico` | `image/x-icon` | Favicons |
| `.txt` | `text/plain` | Text files |
| `.md` | `text/markdown` | Markdown files |
| `.xml` | `application/xml` | XML files |
| `.pdf` | `application/pdf` | PDF documents |
| `.woff` | `font/woff` | WOFF fonts |
| `.woff2` | `font/woff2` | WOFF2 fonts |
| `.ttf` | `font/ttf` | TrueType fonts |
| `.eot` | `application/vnd.ms-fontobject` | EOT fonts |
| Other | `application/octet-stream` | Binary files |

## Adding New Static Files

To add new static files to your application:

1. **Place the file in the `web/` directory**
   ```
   web/
   ├── index.html
   ├── style.css
   ├── app.js
   └── your-new-file.html  ← Add here
   ```

2. **Access via URL**
   ```
   http://localhost:8080/your-new-file.html
   ```

No code changes are required! The server automatically serves any file in the `web/` directory.

## Caching

Static files are served with cache control headers:
```
Cache-Control: public, max-age=3600
```

This allows browsers to cache files for 1 hour, improving performance.

## Error Handling

### 404 Not Found

When a requested file doesn't exist, the server returns:
- HTTP Status: 404
- Content-Type: `text/html`
- Body: HTML error page with user-friendly message

Example:
```html
<!DOCTYPE html>
<html>
<head><title>404 Not Found</title></head>
<body>
  <h1>404 - File Not Found</h1>
  <p>The requested file was not found.</p>
</body>
</html>
```

## Available Static Resources

Current files served by the application:

### Main Application
- `index.html` - Main application interface
- `style.css` - Application styles
- `app.js` - Application JavaScript

### Test Pages
- `test_api_communication.html` - API communication tests
- `test_visualization.html` - Visualization tests
- `test_results.html` - Test results display

### Documentation
- `README.md` - Web interface documentation
- `TESTING_GUIDE.md` - Testing instructions
- `VISUALIZATION_TESTING_GUIDE.md` - Visualization testing guide
- Various verification documents

## Security Considerations

### Path Traversal Protection

The server only serves files from the `web/` directory. Attempts to access files outside this directory (e.g., `../../../etc/passwd`) are blocked by the file system.

### Binary File Support

Files are opened in binary mode to properly handle:
- Images (PNG, JPEG, GIF)
- Fonts (WOFF, TTF, EOT)
- PDF documents
- Other binary assets

## Performance Tips

1. **Use appropriate file formats**
   - Compress images before adding them
   - Minify CSS and JavaScript for production
   - Use modern image formats (WebP, AVIF) when possible

2. **Leverage browser caching**
   - Files are cached for 1 hour by default
   - For frequently changing files, consider cache-busting techniques

3. **Optimize file sizes**
   - Keep HTML/CSS/JS files small
   - Use external CDNs for large libraries when appropriate

## Troubleshooting

### File Not Loading

1. **Check file location**
   - Ensure file is in `web/` directory
   - Verify file name matches URL (case-sensitive on some systems)

2. **Check file permissions**
   - Ensure the server process can read the file

3. **Check browser console**
   - Look for 404 errors
   - Verify MIME type is correct

### Incorrect MIME Type

If a file is served with the wrong MIME type:

1. Check the file extension
2. Add the extension to `get_mime_type()` in `option_pricer_server.cpp`
3. Rebuild the server

## Example Usage

### Serving a New HTML Page

1. Create `web/about.html`:
   ```html
   <!DOCTYPE html>
   <html>
   <head><title>About</title></head>
   <body><h1>About OptionPricer</h1></body>
   </html>
   ```

2. Access at: `http://localhost:8080/about.html`

### Serving an Image

1. Add `web/logo.png`
2. Reference in HTML:
   ```html
   <img src="/logo.png" alt="Logo">
   ```

### Serving Custom CSS

1. Add `web/custom.css`
2. Link in HTML:
   ```html
   <link rel="stylesheet" href="/custom.css">
   ```

## API Endpoints

Note: The following paths are reserved for API endpoints and will NOT serve static files:

- `/api` - API information
- `/api/price` - Option pricing endpoint
- `/api/visualize` - Visualization data endpoint

## Conclusion

The static file serving system is designed to be simple and automatic. Just add files to the `web/` directory and they're immediately accessible via the web server.
