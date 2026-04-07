import processing.serial.*;

Serial myPort;
String data = "";
float angle = 0;
float distance = 0;
String status = "INICIANDO...";
boolean objectDetected = false;
float radarRadius; 

void setup() {
  fullScreen(); 
  
  radarRadius = height * 0.45;
  
  printArray(Serial.list());
  try {
    String portName = Serial.list()[0]; 
    myPort = new Serial(this, portName, 9600);
    myPort.bufferUntil('\n');
  } catch (Exception e) {
    println("Erro: Verifique a porta Serial.");
  }
}

void draw() {
  fill(0, 20); 
  noStroke();
  rect(0, 0, width, height);
  
  drawRadarBackground();
  drawObject();
  drawSweepLine();
  drawUI();
}

void drawRadarBackground() {
  pushMatrix();
  translate(width/2, height - 100);
  noFill();
  strokeWeight(2);
  stroke(0, 100, 0);
  
  for (float d = radarRadius/3; d <= radarRadius * 2; d += radarRadius/1.5) {
    arc(0, 0, d, d, PI, TWO_PI);
  }
  
  for (int i = 0; i <= 180; i += 30) {
    float x = radarRadius * cos(radians(-i));
    float y = radarRadius * sin(radians(-i));
    line(0, 0, x, y);
  }
  popMatrix();
}

void drawSweepLine() {
  pushMatrix();
  translate(width/2, height - 100);
  strokeWeight(4);
  stroke(0, 255, 0);
  float x = radarRadius * cos(radians(-angle));
  float y = radarRadius * sin(radians(-angle));
  line(0, 0, x, y);
  popMatrix();
}

void drawObject() {
  if (distance > 0 && distance <= 40) {
    pushMatrix();
    translate(width/2, height - 100);
    strokeWeight(15);
    stroke(255, 0, 0);
    
    float pixDist = map(distance, 0, 40, 0, radarRadius);
    float x = pixDist * cos(radians(-angle));
    float y = pixDist * sin(radians(-angle));
    point(x, y);
    popMatrix();
  }
}

void drawUI() {
  pushStyle();
  fill(0);
  noStroke();
  rect(0, 0, 400, 150);          
  rect(width/2 - 300, 20, 600, 60); 
  popStyle();

  fill(255);
  textSize(24);
  textAlign(LEFT);
  text("MODO: " + status, 30, 50);
  
  fill(0, 255, 0);
  text("ÂNGULO: " + int(angle) + "°", 30, 85);
  
  if (distance > 0) {
    text("DISTÂNCIA: " + nf(distance, 1, 1) + " cm", 30, 120);
  } else {
    text("DISTÂNCIA: ---", 30, 120);
  }
  
  if (objectDetected) {
    fill(255, 0, 0);
    textSize(36);
    textAlign(CENTER);
    text("⚠ OBJETO DETECTADO ⚠", width/2, 60);
  }
}

void serialEvent(Serial myPort) {
  try {
    data = myPort.readStringUntil('\n');
    if (data != null) {
      data = trim(data);
      if (data.contains("[MANUAL]")) status = "MANUAL";
      if (data.contains("[AUTOMATICO]")) status = "AUTOMÁTICO";
      
      if (data.contains("deg")) {
        objectDetected = data.contains("OBJETO");
        String[] parts = split(data, '|');
        if (parts.length >= 2) {
          angle = float(parts[1].replaceAll("deg", "").trim());
          if (parts.length >= 3) {
            String distStr = parts[2].replaceAll("cm", "").trim();
            distance = (distStr.contains("fora de alcance")) ? -1 : float(distStr);
          }
        }
      }
    }
  } catch (Exception e) {}
}
