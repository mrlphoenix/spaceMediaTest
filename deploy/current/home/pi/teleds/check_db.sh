sqlite3 -line /home/pi/teleds/stat.db 'PRAGMA integrity_check;'
has_integrity=$?
echo $has_integrity
