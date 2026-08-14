#!/usr/bin/env bash
# Convert DOSBox AVI recordings to modern QuickTime / Web compatible MP4

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$SCRIPT_DIR" || exit 1

if ! command -v ffmpeg >/dev/null 2>&1; then
    echo "Error: ffmpeg is not installed. Install via 'brew install ffmpeg'."
    exit 1
fi

mkdir -p capture
found=0

for avi in capture/*.avi; do
    [ -e "$avi" ] || continue
    found=1
    base="$(basename "$avi" .avi)"
    mp4="capture/${base}.mp4"
    echo "Converting $avi -> $mp4..."
    ffmpeg -y -i "$avi" \
        -vf "scale=1440:1080:flags=neighbor,setsar=1" \
        -c:v libx264 -crf 18 -preset medium \
        -c:a aac -b:a 192k \
        -movflags +faststart \
        "$mp4"
    echo "Done: $mp4"
done

if [ "$found" -eq 0 ]; then
    echo "No .avi files found in capture/."
fi
