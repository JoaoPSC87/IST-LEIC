// Target class (position and width)
class Target
{
  constructor(x, y, w, l, id, isFirstOfGroup = false)
  {
    this.x            = x;
    this.y            = y;
    this.width        = w;
    this.label        = l;
    this.id           = id;
    this.isFirstOfGroup = isFirstOfGroup;
  }
  
  // Checks if a mouse click took place
  // within the target
  clicked(mouse_x, mouse_y)
  {
    return dist(this.x, this.y, mouse_x, mouse_y) < this.width / 2;
  }
  
  // Draws the target (i.e., a circle)
  // and its label
   draw()
  {
    // Color by first letter (A-Z mapped to hue 0-360)
    let letter = this.label.charAt(0).toUpperCase();
    let letterIndex = letter.charCodeAt(0) - 65;
    let hue = (letterIndex * 137) % 360;  // golden angle — letras     adjacentes ficam com cores bem distintas
    colorMode(HSB, 360, 100, 100);
    if (selected_ids.has(this.id)) {
      fill(color(hue, 30, 35));
    } else {
      fill(color(hue, 65, 75));
    }
    colorMode(RGB, 255, 255, 255);
    circle(this.x, this.y, this.width);
    
    // // Draw label
    // textFont("Arial", 16);
    // fill(selected_ids.has(this.id) ? color(120, 120, 120) : color(255, 255, 255));
    // textAlign(CENTER);
    // text(this.label, this.x, this.y);

        
    // ---- Draw label ----

    let first = this.label.charAt(0);
    let rest  = this.label.slice(1);

    // measure widths
    push();
    textFont("Arial", 20);
    let w1 = textWidth(first);
    pop();

    push();
    textFont("Arial", 16);
    let w2 = textWidth(rest);
    pop();

    let totalWidth = w1 + w2;
    let startX = this.x - totalWidth / 2;

    fill(selected_ids.has(this.id) ? color(120,120,120) : color(255,255,255));
    textAlign(LEFT, CENTER);

    // first letter (with black outline)
    push();
    textFont("Arial", 26);
    stroke(0);
    strokeWeight(2);
    fill(selected_ids.has(this.id) ? color(120,120,120) : color(255,255,255));
    text(first, startX, this.y);
    pop();

    // rest of label (no outline)
    push();
    textFont("Arial", 18);
    noStroke();
    fill(selected_ids.has(this.id) ? color(120,120,120) : color(255,255,255));
    text(rest, startX + 4 + w1, this.y);
    pop();
  }
}