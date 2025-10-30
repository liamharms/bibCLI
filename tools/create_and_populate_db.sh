rm ../src/db/bible.db

python create_db.py

python populate_canon.py

python populate_strongs.py

python populate_module_with_strongs.py ./resources/ASV.zip
python populate_module_with_strongs.py ./resources/Byz.zip
python populate_module_with_strongs.py ./resources/KJV.zip
python populate_module_with_strongs.py ./resources/KJVA.zip
python populate_module_with_strongs.py ./resources/LXX.zip
python populate_module_with_strongs.py ./resources/OSHB.zip
python populate_module_with_strongs.py ./resources/TR.zip