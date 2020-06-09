# RG1-let-us-get-some-buckets
Let us get some buckets je igra odradjena kao imitacija dela igre kosarke u 2D stilu.

# Instalacija
```shell
git clone https://github.com/MATF-RG19/RG1-let-us-get-some-buckets.git
cd RG1-let-us-get-some-buckets
```
Igra se instalira se pokretanjem Makefile komandom make.<br>
Nakon toga se pokrece komandom ./LetsGetSomeBuckets.<br>
Od biblioteka je potrebno imati OpenGL. Ona se na Linux-u download-uje i instalira komandom:
```shell
sudo apt-get install freeglut3-dev
```

# Opis
Cilj igre je postici sto vise poena, odnosno ubaciti loptu kroz obruc sto vise puta, a da se pritom ne udari loptom u plafon ili pod. Takodje, pri svakom prolasku obruca mora se postici pogodak inace je kraj igre.

# Komande
<kbd>G</kbd> / <kbd>g</kbd> Pocetak igre.<br>
<kbd>M</kbd> / <kbd>m</kbd> Skok lopte.<br>
<kbd>B</kbd> / <kbd>b</kbd> Postavljanje pozadine.<br>
<kbd>W</kbd> / <kbd>w</kbd> Gasenje pozadine.<br>
<kbd>S</kbd> / <kbd>s</kbd> Stopiranje igre.<br>
<kbd>R</kbd> / <kbd>r</kbd> Restartovanje igre.<br>
<kbd>ESC</kbd> Izlaz iz igre.