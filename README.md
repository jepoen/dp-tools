# dp-tools

Simple tools for content providing (CP) on pgdp.net, especially for German
fraktur book, so the rest of the documentation is provided in German.

## handleTess

Nachbehandlung des Tesseract-Ergebnisses.

Vorgehen:

Erstellen einer Tesseract-Steuerdatei, die sowohl Text- als Box-Daten
erzeugt, Ablage unter `tessdata/config/boxtext`:

```
tessedit_create_txt 1
tessedit_create_boxfile 1
```

Erstellen eines Verzeichnisses für die Tesseract-Ausgabe mit Name `in`
und Tesseract-Aufruf:

```shell
mkdir $OCRDIR/in
tesseract file $OCRDIR/in/file -l deu_frak boxtext
```

Nachbehandlung mit `handleTess`:

```shell
cd $OCRDIR
handleTess -f
```

Ergebnis steht in `$OCRDIR/out`, Seiten werden von 001 bis nnn neu numeriert.

## renumber

Neunumerieren von Dateien. Für die Dateinamen mit gleicher Erweiterung werden
in alphabetischer Reihenfolge symbolische Links 001 bis nnn erzeugt und
die Erweiterung wieder angehängt.

Aufruf:

```shell
renumber extension target_directory
```

## topng.py

Umwandlung der Scantailor-Tiff-Images in verkleinerte PNG-Dateien. Verwendet
`convert` (ImageMagick) und `pngcrush`.

Aufruf:

```shell
python topng.py tifDir pngDir [scale]
```
## Calamari-venv

* Cuda installieren (11.8)
* Miniconda installieren
* Dann:

```shell
. .../venv/bin/activate
conda update conda --all
conda update anaconda
conda install python=3.9
pip install calamari-ocr
```

Tensorflow etc. nicht mit `conda` installieren!

## Calamari-OCR

Vorgehen:

* Seiten in 300-dpi-PNG mit Tiefe 1 umwandeln
* Seite in Zeilen segmentieren (Kraken oder `tesseract lstmbox`),
  besser Zeichenboxen erzeugen und diese zusammensetzen (`ocr-tesslineboxes`)
* Zeilen mit `calamari predict` erkennen
* Zeilen zusammensetzen, Absatzheuristik über JSON-Boxfile
* Postprocessing: Abteilungen zusammenziehen, Zeichen ersetzen

## Calamari-Training

* Ground-Truth-Files aus Zeilenbildern erzeugen
