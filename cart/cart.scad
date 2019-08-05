resolution=50;

platform_length=252.2;
platform_outer_length=260;
platform_width=636;
leg_length=410;
leg_width=48;
material=10;
plastic_material=3;

module wheel(r=60, pinr=2.5, thickness=10) {
    difference() {
        union() {
            cylinder(h=thickness, r=r, $fn=resolution);
            translate([0,0,thickness]) cylinder(h=thickness, r=r-thickness, $fn=resolution);
            }
        translate([0, 0, -thickness]) cylinder(h=thickness*4, r=pinr, $fn=resolution);
        }
    }

module cogwheel(r=40, h=material, cognr=10, cogr=50, pinr=2.5) {
    difference() {
        cylinder(r=r, h=h);
        for (pos = [0 : 360/cognr : 360]) {
            translate([cogr*cos(pos), cogr*sin(pos), -h]) cylinder(r=2*PI*cogr / cognr / 2, h=h*3);    
        }
        translate([0, 0, -h]) cylinder(h=h*3, r=pinr, $fn=resolution);
    }
}

module platform(platform_length=252.2, platform_outer_length=260, platform_width=636, leg_length=410, leg_width=48, material=10) {
    leg_spacing = platform_width - 2*leg_width;
    translate([0, 0, -material]) cube([leg_width, leg_length, material]);
    translate([leg_width+leg_spacing, 0, -material]) cube([leg_width, 410, material]);
    rotate(120, [1,0,0]) translate([0, -(platform_outer_length-platform_length), 0]) cube([platform_width, platform_outer_length, material]);
}

module track(gauge=140, width=30, height=30, platform_width=platform_width, leg_width=leg_width) {
    translate([0, -width, 0]) cube([platform_width, width, material]);
    translate([leg_width, -width, -height]) cube([platform_width-2*leg_width, material, height]);
    translate([0, gauge, 0]) cube([platform_width, width, material]);
    translate([leg_width, gauge+width-material, -height]) cube([platform_width-2*leg_width, material, height]);
}

module cart_wheels(radius=30, length=200, track_width=140, pinr=2.5) {
    translate([0,material,radius]) rotate(90, [1, 0, 0]) wheel(radius, pinr, material);
    translate([length,material,radius]) rotate(90, [1, 0, 0]) wheel(radius, pinr, material);
    translate([0,track_width-material,radius]) rotate(-90, [1, 0, 0]) wheel(radius, pinr, material);
    translate([length,track_width-material,radius]) rotate(-90, [1, 0, 0]) wheel(radius, pinr, material);
    
    translate([length/2,material,-radius+material]) rotate(90, [1, 0, 0]) wheel(radius, pinr, material);
    translate([length/2,track_width-material,-radius+material]) rotate(-90, [1, 0, 0]) wheel(radius, pinr, material);
}

module cart_boogie(radius=30, length=200, track_width=140, pinr=2.5) {
    cart_wheels(radius=radius, length=length, track_width=track_width, pinr=pinr);
    color([0, 0, 1]) translate([0, -material, radius]) rotate(-90, [1, 0, 0]) cylinder(r=pinr, h=track_width+2*material);
    color([0, 0, 1]) translate([length, -material, radius]) rotate(-90, [1, 0, 0]) cylinder(r=pinr, h=track_width+2*material);
    color([0, 0, 1]) difference() {
        translate([length/2, -material, -radius+material]) rotate(-90, [1, 0, 0]) cylinder(r=pinr, h=track_width+2*material);
        translate([length/2-2*pinr, 3*material, -radius+material-2*pinr]) cube([pinr*4, track_width-6*material, pinr*3]);
    }
}

module cart_base(wheel_radius=30, length=200, track_width=140, pipe_radius=25) {
    translate([-wheel_radius, material, 0])
        translate([0,material,0]) cube([length+2*wheel_radius, track_width-material*4, material]);
    translate([-wheel_radius, material, -2*wheel_radius+material]) cube([length+2*wheel_radius, material, wheel_radius*4-material]);
    translate([-wheel_radius, track_width-material-material, -2*wheel_radius+material]) cube([length+2*wheel_radius, material, wheel_radius*4-material]);
}

module sleeve(wheel_radius=wheel_radius, length=length, track_width=track_width, pipe_radius=pipe_radius) {
    translate([length/2, track_width/2, material]) difference() {
        union() {
          cylinder(r=pipe_radius+plastic_material, h=wheel_radius*2-material);
          cylinder(r=pipe_radius+plastic_material+material, h=material);
        }
        translate([0, 0, -1]) cylinder(r=pipe_radius, h=wheel_radius*2-material+2);
        translate([-pipe_radius-plastic_material-1,-material/2-plastic_material,material])
            cube([(pipe_radius+plastic_material)*2+2,material+2*plastic_material,wheel_radius*2-material+2]);
    }
}

module pipewheels(wheel_radius=30, length=200, track_width=140, pipe_radius=25, cogwheelr=20, pinr=2.5) {
    translate([length/2-pipe_radius-cogwheelr+2,track_width/2-material/2,2*wheel_radius-cogwheelr]) rotate(-90, [1,0,0]) cogwheel(r=cogwheelr, cogr=cogwheelr+2, cognr=20);
    translate([length/2+pipe_radius+cogwheelr-2,track_width/2-material/2,2*wheel_radius-cogwheelr]) rotate(-90, [1,0,0]) cogwheel(r=cogwheelr, cogr=cogwheelr+2, cognr=20);

    translate([length/2-pipe_radius-cogwheelr+2,0,2*wheel_radius-cogwheelr]) rotate(-90, [1, 0, 0]) cylinder(r=pinr, h=track_width);
    translate([length/2+pipe_radius+cogwheelr-2,0,2*wheel_radius-cogwheelr]) rotate(-90, [1, 0, 0]) cylinder(r=pinr, h=track_width);
}

module cart(wheel_radius=30, length=200, track_width=140, pipe_radius=25, cogwheelr=20) {
    difference() {
        cart_base(wheel_radius=wheel_radius, length=length, track_width=track_width, pipe_radius=pipe_radius);
        cart_boogie(radius=wheel_radius, length=length, track_width=track_width, pinr=2.5);
        pipewheels(wheel_radius, length, track_width, pipe_radius, cogwheelr);
        translate([length/2, track_width/2, -material]) cylinder(r=pipe_radius, h=3*material);
    }

    color([.5, .5, 1]) sleeve(wheel_radius, length, track_width, pipe_radius);
    color([1,1,1]) pipewheels(wheel_radius, length, track_width, pipe_radius, cogwheelr);    
    cart_boogie(radius=wheel_radius, length=length, track_width=track_width, pinr=2.5);    
}


color([1, 0, 0]) platform();
translate([0,150,0]) track(width=30);
translate([200,150,0]) cart();