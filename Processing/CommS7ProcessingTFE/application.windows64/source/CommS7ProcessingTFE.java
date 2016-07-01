import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 
import controlP5.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class CommS7ProcessingTFE extends PApplet {

// TFE Brahim CR2ATION
 //<>// //<>// //<>// //<>// //<>// //<>// //<>// //<>//
 // importe la librairie GUI controlP5
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

ControlP5 cp5;    // d\u00e9clare un objet principal de la librairie  controlP5
ListBox l;
Textlabel Tl;

int TransmissionSQ=0;
int cnt = 0;
int compteur=0;
String Recu="";

BufferedReader reader;
String line;

Toggle togglePause; // d\u00e9clare un/des objets Toggle

Slider sliderCoeff; // d\u00e9clare un/ des objet Slider
Slider sliderBase; // d\u00e9clare un/ des objet Slider

ControlWindow cw1; // d\u00e9clare un/des objet fenetre ControlWindow 

String[] TextValue = new String[100];
String[] men1= new String[100];
String OKPROC="OKPROC";
String OKFIN="OKFIN";  

//------ d\u00e9claration des variables de couleur utiles ----
int jaune=color(255, 255, 0);
int vert=color(0, 255, 0);
int rouge=color(255, 0, 0);
int bleu=color(0, 0, 255);
int noir=color(0, 0, 0);
int blanc=color(255, 255, 255);
int bleuclair=color(0, 255, 255);
int violet=color(255, 0, 255);

// --- port S\u00e9rie ---
Serial  myPort; // Cr\u00e9ation objet d\u00e9signant le port s\u00e9rie
int PasApas=0;

//******************************************************************************************************
public void setup() {
  

  println(Serial.list()); // affiche dans la console la liste des ports s\ufffdries
  myPort = new Serial(this, Serial.list()[0], 9600); // Initialise une nouvelle instance du port S\ufffdrie
  myPort.bufferUntil('\n'); // attendre arriv\ufffde d'un saut de ligne pour g\ufffdn\ufffdrer \ufffdv\ufffdnement s\ufffdrie 
  myPort.clear();
  ControlP5.printPublicMethodsFor(ListBox.class);
  // --- police texte --- 
  PFont font = createFont("arial", 20); // cr\u00e9e un objet de police texte


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
public void draw() {

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

public void AffichageTags(String[] tags) {  // This function also does not return a value
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
public void serialEvent(Serial p) { 
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



public void controlEvent(ControlEvent theEvent) {
  if (theEvent.isAssignableFrom(Textfield.class)) {
    println("controlEvent: accessing a string from controller "
      +theEvent.getName()+"': "
      +theEvent.getStringValue());
  }
  if (theEvent.getName().equals("myList")) {
  }
}


//---------------
public void mousePressed() {
  //print("X:"+mouseX);
  //print("Y:"+mouseY);
}

public void input(String theText) {
  // automatically receives results from controller input
  println("a textfield event for controller 'input' : "+theText);
}

public void nettoyer() {

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
  public void settings() {  size(1000, 600); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "CommS7ProcessingTFE" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
