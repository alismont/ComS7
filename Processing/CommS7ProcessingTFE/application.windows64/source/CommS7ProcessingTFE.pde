// TFE Brahim CR2ATION
import processing.serial.*; //<>// //<>// //<>// //<>// //<>// //<>// //<>// //<>//
import controlP5.*; // importe la librairie GUI controlP5
/**
 * ControlP5 Textfield
 *
 *
 * find a list of public methods available for the Textfield Controller
 * at the bottom of this sketch.
 *
 * by Andreas Schlegel, 2012
 * www.sojamo.de/libraries/controlp5
 *
 */

PrintWriter output;
PFont myFont;     // The display font
String inString;  // Input string from serial port
int lf = 10;      // ASCII linefeed 
String t=":";
String[] tag = new String[110];
int MEMOSYN=0;
String TramEcriture="";
int i=0;

ControlP5 cp5;    // déclare un objet principal de la librairie  controlP5
ListBox l;
Textlabel Tl;

int TransmissionSQ=0;
int cnt = 0;
int compteur=0;
String Recu="";

BufferedReader reader;
String line;

Toggle togglePause; // déclare un/des objets Toggle

Slider sliderCoeff; // déclare un/ des objet Slider
Slider sliderBase; // déclare un/ des objet Slider

ControlWindow cw1; // déclare un/des objet fenetre ControlWindow 

String[] TextValue = new String[100];
String[] men1= new String[100];
String OKPROC="OKPROC";
String OKFIN="OKFIN";  

//------ déclaration des variables de couleur utiles ----
int jaune=color(255, 255, 0);
int vert=color(0, 255, 0);
int rouge=color(255, 0, 0);
int bleu=color(0, 0, 255);
int noir=color(0, 0, 0);
int blanc=color(255, 255, 255);
int bleuclair=color(0, 255, 255);
int violet=color(255, 0, 255);

// --- port Série ---
Serial  myPort; // Création objet désignant le port série
int PasApas=0;

//******************************************************************************************************
void setup() {
  size(1000, 600);

  println(Serial.list()); // affiche dans la console la liste des ports s�ries
  myPort = new Serial(this, Serial.list()[0], 9600); // Initialise une nouvelle instance du port S�rie
  myPort.bufferUntil('\n'); // attendre arriv�e d'un saut de ligne pour g�n�rer �v�nement s�rie 
  myPort.clear();
  ControlP5.printPublicMethodsFor(ListBox.class);
  // --- police texte --- 
  PFont font = createFont("arial", 20); // crée un objet de police texte


  cp5 = new ControlP5(this);


  cp5.addTextfield("DB")
    .setPosition(20, 170)
    .setSize(100, 20)
    .setFont(createFont("arial", 10))
    .setAutoClear(false)
    ;

  //
  //cp5.addTextfield("COMPTEUR")
  //  .setPosition(20, 220)
  //  .setSize(100, 20)
  //  .setFont(createFont("arial", 10))
  //  .setAutoClear(false)
  //  ;
  //

  Tl= cp5.addTextlabel("label")
    .setText("Compteur: "+str(compteur))
    .setPosition(225, 50)
    .setColorValue(0xffffff00)
    .setFont(createFont("Georgia", 20))
    ;

  cp5.addBang("clear")
    .setPosition(130, 160)
    .setSize(80, 20)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ;  

  cp5.addBang("Add")
    .setPosition(130, 190)
    .setSize(80, 20)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ;  

  cp5.addBang("Send")
    .setPosition(600, 80)
    .setSize(80, 20)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ;  

  cp5.addBang("Remove")
    .setPosition(600, 50)
    .setSize(80, 20)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ; 

  cp5.addBang("EXIT")
    .setPosition(600, 400)
    .setSize(80, 20)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ;

  cp5.addBang("SAVE")
    .setPosition(600, 110)
    .setSize(80, 20)
    .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
    ;

  l = cp5.addListBox("myList")
    .setPosition(400, 50)
    .setSize(120, 420)
    .setItemHeight(20)
    .setBarHeight(20)
    .setColorBackground(color(255, 128))
    .setColorActive(color(0))
    .setColorForeground(color(255, 100, 0))
    ;

  l.getCaptionLabel().toUpperCase(true);
  l.getCaptionLabel().set("DB NAME:");
  l.getCaptionLabel().setColor(0xffff0000);
  textFont(font);

  LOAD();

  background(0);
  textSize(32);
  fill(255, 100, 100);
  text("CHOIX DES DB(s)", 380, 28);
}

//****************************************************************************
void draw() {

  if (inString != null) {
    String[] q = splitTokens(inString, "/");
    // println(inString);
    //println(q.length + " values found");  // Prints "4 values found"
    tag[0]=q[0];//
    //tag[1]=q[1];//
  }
  AffichageTags(tag);

  if (TransmissionSQ==1) {
    Transmission();
  }
}
//****************************************************************************

void AffichageTags(String[] tags) {  // This function also does not return a value
  background(0);

  if (tags[0] != null) {
    //image(img, 0, 0); 
    fill(250, 226, 5);
    textSize(20);
    Recu=trim(tags[0]);
    text("Recu:"+tags[0], 26, 350);
  }
}
//EVENT RS232 ARDUINO<->PROCESSING
void serialEvent(Serial p) { 
  inString =p.readString();
  print(inString);
}

public void clear() {
  cp5.get(Textfield.class, "DB").clear();
}

public void Add() {

  l.addItem(cp5.get(Textfield.class, "DB").getText(), cnt);
  l.getItem(cp5.get(Textfield.class, "DB").getText()).put("color", new CColor().setBackground(0xffff0000).setBackground(0xffff8800));

  TextValue[compteur]=(cp5.get(Textfield.class, "DB").getText());
  compteur++; 
  nettoyer();
  background(0);
  textSize(32);
  fill(255, 100, 100);
  text("CHOIX DES DB(s)", 380, 28);
  Tl.setText("Compteur: "+str(compteur));
  println("le nombre des DB sont :"+compteur);
  cp5.get(Textfield.class, "compteur").getText();
}

public void Send() {

  TransmissionSQ=1;
}

public void Transmission() {

  switch(PasApas) {
  case 0: 
    println("Zero");  
    PasApas=1;
    tag[0]="";
    Recu="";
    myPort.write("OKPROC/");
    break;
  case 1: 
    println("One"); 
    //pas  
    if (Recu != "") {
      println(Recu.substring(0)); 
      if (OKPROC.equals(Recu) == true) {
        PasApas=2;
        Recu="";
      }
    }
    break;
  case 2: 
    println("deux"); 
    break;
  case 3: 
    println("trois");  
    PasApas=4;
    myPort.write("OKFIN/");
    break;
  case 4: 
    println("QUATRE"); 
    if (Recu != "") {
      println(Recu.substring(0)); 
      if (OKFIN.equals(Recu) == true) {
        TransmissionSQ=0;
        Recu="";
        println("FIN Pas");
        myPort.write("/");
      }
    }
    break;
  }


  //pas 2
  if (PasApas==2) {
    reader = createReader("DBs.txt");
    i=0;
    for (i=0; i<100; i++) {
      try {
        line = reader.readLine();
      } 
      catch (IOException e) {
        e.printStackTrace();
        line = null;
      }

      if (line == null) {
        // Stop reading because of an error or file is empty
        i=100;
        PasApas=3;
        break;
      } else {

        //myPort.write(i+'/');
        myPort.write(line+'/');
        println(line);
        i++;
      }
    }
  }
}




public void Remove() {
  if (cp5.get(Textfield.class, "DB").getText()!=null) {
    l.removeItem(cp5.get(Textfield.class, "DB").getText());
    println("All Items"+l.getItems());
    nettoyer();
    background(0);
    textSize(32);
    fill(255, 100, 100);
    text("CHOIX DES DB(s)", 380, 28);
    compteur=compteur-1;

    Tl.setText("Compteur: "+str(compteur));

    println("le nombre des DB sont :"+compteur);
  }
}



void controlEvent(ControlEvent theEvent) {
  if (theEvent.isAssignableFrom(Textfield.class)) {
    println("controlEvent: accessing a string from controller "
      +theEvent.getName()+"': "
      +theEvent.getStringValue());
  }
  if (theEvent.getName().equals("myList")) {
  }
}


//---------------
void mousePressed() {
  //print("X:"+mouseX);
  //print("Y:"+mouseY);
}

public void input(String theText) {
  // automatically receives results from controller input
  println("a textfield event for controller 'input' : "+theText);
}

void nettoyer() {

  background(255);
}

public void EXIT() {
  output = createWriter("DBs.txt");
  i=0;
  for (i=0; i<=compteur; i++) {
    if (i==0) {
      output.println(compteur);
    } else {
      output.println( cp5.get(ListBox.class, "myList").getItem(i-1).get("text"));
    }
  }
  output.close(); // Finishes the file
  exit(); // Stops the program
}

public void LOAD() {
  reader = createReader("DBs.txt");
  compteur=0;
  i=0;
  for (i=0; i<100; i++) {
    try {
      line = reader.readLine();
    } 
    catch (IOException e) {
      e.printStackTrace();
      line = null;
    }

    if (line == null) {
      // Stop reading because of an error or file is empty
      i=100;
      break;
    } else {
      //String[] pieces = split(line, TAB);
      if (i>0) {
        println(line);
        l.addItem(line, i-1);
        l.getItem(line).put("color", new CColor().setBackground(0xffff0000).setBackground(0xffff8800));
      }
      i++;
    }

    compteur++;
  }
  if (compteur>1) {
    compteur--;
  }
  Tl.setText("Compteur: "+str(compteur));
}



public void SAVE() {
  output = createWriter("DBs.txt"); 
  i=0;
  for (i=0; i<=compteur; i++) {
    if (i==0) {
      output.println(compteur);
    } else {
      output.println( cp5.get(ListBox.class, "myList").getItem(i-1).get("text"));
    }
  }
  output.close(); // Finishes the file
}