#!/bin/bash

newlog="debian/changelog.new"
oldlog="debian/changelog"

echo "djview4poliqarp (4.7-99-snapshot-$(date +%F-%H-%M)) unstable; urgency=low" > "$newlog"
echo "" >> "$newlog"
echo "  * snapshot of $(git log --pretty=format:'%H' --date=iso | head -1)" >> "$newlog"
echo "" >> "$newlog"
echo " -- Grzegorz Chimosz <Grzegorz.Chimosz@gmail.com>  $(date -R)" >> "$newlog"
echo "" >> "$newlog"
cat "$oldlog" >> "$newlog"

mv "$newlog" "$oldlog"

debuild -b -uc -us

git checkout debian/changelog # cleanup
