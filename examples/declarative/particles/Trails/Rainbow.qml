import Qt.labs.particles 2.0
import QtQuick 2.0

Rectangle {
    id: root
    width: 360
    height: 540
    color: "black"

    ParticleSystem{ id: particles }
    ColoredParticle{
        system: particles
        colorVariation: 0.5
        additive: 1

        image: "content/particle.png"
        colortable: "content/colortable.png"
    }
    TrailEmitter{
        system: particles
        particlesPerSecond: 500
        particleDuration: 2000

        y: root.height / 2 + Math.sin(t * 2) * root.height * 0.3
        x: root.width / 2 + Math.cos(t) * root.width * 0.3
        property real t;

        NumberAnimation on t {
            from: 0; to: Math.PI * 2; duration: 10000; loops: Animation.Infinite
        }

        speedFromMovement: 20

        speed: PointVector{ xVariation: 5; yVariation: 5;}
        acceleration: PointVector{ xVariation: 5; yVariation: 5;}

        particleSize: 2
        particleEndSize: 8
        particleSizeVariation: 8
    }
}