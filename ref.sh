sed "s/Units,/Units\&,/g" src/Units.h -i
sed "s/Units)/Units\&)/g" src/Units.h -i
sed "s/Units units,/Units\& units,/g" src/Units.c -i
sed "s/Units units)/Units\& units)/g" src/Units.c -i
