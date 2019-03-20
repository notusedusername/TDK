/*
*	A program célja, hogy a BrainB program kimeneteként generált .txt
* állományokat
*	nagyobb mennyiségű mérés esetén hatékonyan feldolgozhassuk. A szöveges
* állomány
*	found2lost és lost2found vektorait átlagolja. Futtatáskor a paraméterül
* adott
* path alatt lévő könyvtárfa bejárásával megvizsgálja a .txt fájlokat az
* említett
*	vektorokat keresve, majd ha a mérés nem hibás, akkor feldolgozza.
*	A kimenet 2 vektor, amelyet egy R script segítségével ábrázolunk.
*
*   BUILD: g++ measure_merge.cpp -o measure_merge -lboost_system -lboost_filesystem
* 
*   RUN: ./measure_merge [absolute path](optional) [treshold]
*/

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <numeric>

using namespace boost::filesystem;
using namespace std;

// Köztes lépés a fejlesztés során, a mátrix hasoníltására használtuk (diff)
// std::ofstream ofs ("merge.txt", std::ofstream::out);

vector<vector<int>> matrix;
vector<double> avgBPS;

/*
* A lost2found és found2lost vektorok merge-elése, egy mátrixba,
* a 2 vektort üríti a végén, konzisztenciaőrzés miatt.
*/
void merge(vector<int>& l2f, vector<int>& f2l) {
  vector<int> temp;
  for (int i{0}; i < l2f.size(); ++i) {
    temp.push_back(l2f[i]);
    if (i < f2l.size()) {
      temp.push_back(f2l[i]);
    }
  }
  matrix.push_back(temp);

  l2f.clear();
  f2l.clear();

  // ofs<<"\n";
}

/*
*	Kivonja az 1. argumentum stringből a számokat, és a 2. argumentum
* 	integer vektorban tárolja el.
*/
void extractIntegerWords(string str, vector<int>& out) {
  stringstream ss;
  ss << str;
  string temp;
  int found;
  while (!ss.eof()) {
    ss >> temp;

    if (stringstream(temp) >> found) {
      out.push_back(found);
    }

    temp = "";
  }
}

/*
* Az 1 paraméterként adott path-en található fájlból szedi ki
* a "found2lost:", illetve a "lost2found:" kezdetű sorokat, mivel
* a feldolgozás szempontjából számunkra ezek relevánsak. A
* kinyert adatot a megfelelő nevű 2. és 3. paraméter integer vektorban tárolja.
* A hibás mérések nem tartalmaznak semmit ezekben a sorokban a keresett
* karakterláncok után,
* ezeket ignorálja a program.
*/
int getFileContent(boost::filesystem::path fileName,
                    vector<int>& l2f,
                    vector<int>& f2l) {
  std::ifstream in(fileName.c_str());

  if (!in) {
    std::cerr << "Cannot open the File : " << fileName << std::endl;
  }
  bool flag = false;
  std::string str;

  while (std::getline(in, str)) {
    if (str.find("lost2found:") == 0) {
      if (str.size() > 13) {
        flag = true;
        extractIntegerWords(str, l2f);
      }
    }

    if (str.find("found2lost:") == 0) {
      if (str.size() > 13) {
        extractIntegerWords(str, f2l);
      }
    }
    if(str.find("U R about ") == 0){
      avgBPS.push_back(stod(str.substr(10, str.find(" Kilobytes"))));
    }
  }
  int toReturn = 0;
  if (flag){
    toReturn = l2f.size() + f2l.size();
    merge(l2f, f2l);
  }

  in.close();
  return toReturn;
}
/*
* Visszaadja a paraméterként adott elérési úton lévő fájl
* kiterjesztését, ha nincs akkor a "" stringet.
*/
string getExt(path p) {
  if (p.has_extension()) {
    return p.extension().string();
  }
  return "";
}
/*
* Megvizsgálja a p path-t, ha mappa rekurzívan magát hívja meg,
* ha .txt fájl, akkor meghívja a getFileContent(path, vector<int>, vector<int>)
* függvényt.
*/
void dirent(path p, vector<int>& l2f, vector<int>& f2l, int treshold) {
  int eventCount = 0;
  try {
    if (exists(p)) {
      if (is_regular_file(p)) {
        if (getExt(p) == ".txt" || getExt(p) == ".TXT") {
          eventCount = getFileContent(p, l2f, f2l);
          if (eventCount > treshold ){
            cout<<"\nMagas eseményszám: "<<p<<endl;
          }
        }
      }

      else if (is_directory(p)) {
        for (directory_entry& x : directory_iterator(p)) {
          dirent(x.path(), l2f, f2l, treshold);
        }
      }
    }

  }

  catch (const filesystem_error& ex) {
    cout << ex.what() << '\n';
  }
}

/*
* Kiszámolja, hogy az adott eseményszámnál hány alanyt mért még a program,
* és a résztvevők száma alapján átlagolja az értékeket. Paramétere a mátrix,
* a mátrix vizsgálandó sora (az x. esemény), és a mért résztvevők száma.
*/
double avg(vector<vector<int>>& matrix, int column, int& divisor) {
  double sum = 0;

  for (int i = 0; i < matrix.size(); i++) {
    if ((column >= matrix[i].size())) {
      divisor--;
    } else {
      sum = sum + matrix[i][column];
    }
  }
  sum = sum / divisor;
  return sum;
}

/*
* Kiszámolja az átlagos bps-t.
*/
double countAvgBPS(){
  double sum_of_elements = 0;

  if(avgBPS.size() == 0){
  	cout<<"Nincs átlaglolható érték"<<endl;
  	return 0;
  }
  else{
  sum_of_elements = accumulate(avgBPS.begin(), avgBPS.end(), 0.);
  return sum_of_elements / avgBPS.size();
  }
}

int main(int argc, char* argv[]) {
  path p;

  if (argc >= 2) {
    p = argv[1];
  } else {
    p = boost::filesystem::current_path();
  }

  /* A második argumentummal megadhatunk egy küszöbértéket, ami a
  * kiugró eseményszámot tartalmazó méréseket tartalmazza, mert ezek 
  * valószínűleg hibás mérések (például sokszor felengedte az egérgombot),
  * de legfőképpen, a grafikon végét ezek az eredmények torzítják.
  */
  int treshold = 0;
  if (argc >= 3) {
    treshold = stoi(argv[2]);
  } else {
    //10 perc alatt a 100 találat-vesztés kiugró érték
    treshold = 100;
  }

  std::vector<int> l2f;
  std::vector<int> f2l;

  dirent(p, l2f, f2l, treshold);
  int maxColumns = 0;
  int divisor = matrix.size();

  // maximum megkeresése
  for (int i = 0; i < matrix.size(); i++) {
    if (matrix[i].size() > maxColumns) {
      maxColumns = matrix[i].size();
    }
  }

  std::vector<double> average;
  std::vector<int> div;

  for (int i = 0; i < maxColumns; i++) {
    divisor = matrix.size();
    // az avg()-ben kiszámolt értékek letárolása
    average.push_back(avg(matrix, i, divisor));

    div.push_back(divisor);
  }

  for (int i = 0; i < div.size(); i++) {
    cout << div[i] << ", ";
  }

  /*
  * kiiratás, az R számára, a vonal alatt az átlagolt eseményértékek,
  * felette a mért részvevők száma.
  */
  cout << " ------- \n";
  for (int i = 0; i < average.size(); i++) {
    cout << average[i] << ", ";
  }
  cout << "\n";
  cout<<"Az átlagos bps a mért csoportban: "<<countAvgBPS()<<endl;
  cout<<"Leggyengébb: "<<*min_element(avgBPS.begin(), avgBPS.end())<<"KB"<<endl;
  cout<<"Legerősebb: "<<*max_element(avgBPS.begin(), avgBPS.end())<<"KB"<<endl;
  return 0;
}
/*
Az R script a program kimenetének feldolgozására:

pdf("udprognum.pdf")

size=c([Ide a div vektor elemei])

plot(size, type="o")
dev.off()

pdf("udprogmeanFL.pdf")

bps=c([Ide az average vektor elemei])
names = c("F", "L")

plot(bps, type="o")
ppp=c(1,3)
text(bps, names, cex=0.9, font=2, pos=ppp)
dev.off()
*/
