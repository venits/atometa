import React, { useRef, useState, useEffect } from 'react';

const WaterMolecule3D = () => {
  const canvasRef = useRef(null);
  const [rotation, setRotation] = useState({ x: 0, y: 0 });
  const [isDragging, setIsDragging] = useState(false);
  const [lastMouse, setLastMouse] = useState({ x: 0, y: 0 });

  // Atom positions (Angstroms)
  const atoms = [
    { symbol: 'O', pos: [0, 0, 0], color: '#FF0505', radius: 0.66 },
    { symbol: 'H', pos: [0.96, 0, 0], color: '#FFFFFF', radius: 0.31 },
    { symbol: 'H', pos: [-0.24, 0.93, 0], color: '#FFFFFF', radius: 0.31 }
  ];

  const bonds = [
    { from: 0, to: 1 },
    { from: 0, to: 2 }
  ];

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    const width = canvas.width;
    const height = canvas.height;

    // 3D to 2D projection
    const project = (x, y, z, rotX, rotY) => {
      // Rotate around Y axis
      const cosY = Math.cos(rotY);
      const sinY = Math.sin(rotY);
      const x1 = x * cosY - z * sinY;
      const z1 = x * sinY + z * cosY;

      // Rotate around X axis
      const cosX = Math.cos(rotX);
      const sinX = Math.sin(rotX);
      const y1 = y * cosX - z1 * sinX;
      const z2 = y * sinX + z1 * cosX;

      // Perspective projection
      const scale = 200 / (4 + z2);
      return {
        x: width / 2 + x1 * scale,
        y: height / 2 - y1 * scale,
        z: z2
      };
    };

    // Clear canvas
    ctx.clearRect(0, 0, width, height);

    // Draw bonds
    ctx.strokeStyle = '#888';
    ctx.lineWidth = 3;
    bonds.forEach(bond => {
      const from = project(...atoms[bond.from].pos, rotation.x, rotation.y);
      const to = project(...atoms[bond.to].pos, rotation.x, rotation.y);
      
      ctx.beginPath();
      ctx.moveTo(from.x, from.y);
      ctx.lineTo(to.x, to.y);
      ctx.stroke();
    });

    // Sort atoms by depth (painter's algorithm)
    const sortedAtoms = atoms.map((atom, i) => {
      const proj = project(...atom.pos, rotation.x, rotation.y);
      return { ...atom, proj, index: i };
    }).sort((a, b) => a.proj.z - b.proj.z);

    // Draw atoms
    sortedAtoms.forEach(atom => {
      const scale = 200 / (4 + atom.proj.z);
      const radius = atom.radius * scale * 50;

      // Shadow
      ctx.fillStyle = 'rgba(0, 0, 0, 0.2)';
      ctx.beginPath();
      ctx.arc(atom.proj.x + 2, atom.proj.y + 2, radius, 0, Math.PI * 2);
      ctx.fill();

      // Atom sphere (with gradient for 3D effect)
      const gradient = ctx.createRadialGradient(
        atom.proj.x - radius/3, 
        atom.proj.y - radius/3, 
        0,
        atom.proj.x, 
        atom.proj.y, 
        radius
      );
      gradient.addColorStop(0, atom.color);
      gradient.addColorStop(1, atom.color === '#FFFFFF' ? '#CCCCCC' : '#AA0000');

      ctx.fillStyle = gradient;
      ctx.beginPath();
      ctx.arc(atom.proj.x, atom.proj.y, radius, 0, Math.PI * 2);
      ctx.fill();

      // Outline
      ctx.strokeStyle = '#000';
      ctx.lineWidth = 1;
      ctx.stroke();

      // Label
      ctx.fillStyle = '#000';
      ctx.font = 'bold 14px Arial';
      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';
      ctx.fillText(atom.symbol, atom.proj.x, atom.proj.y);
    });
  }, [rotation]);

  // Auto-rotation
  useEffect(() => {
    if (!isDragging) {
      const interval = setInterval(() => {
        setRotation(prev => ({
          x: prev.x,
          y: prev.y + 0.01
        }));
      }, 16);
      return () => clearInterval(interval);
    }
  }, [isDragging]);

  const handleMouseDown = (e) => {
    setIsDragging(true);
    setLastMouse({ x: e.clientX, y: e.clientY });
  };

  const handleMouseMove = (e) => {
    if (!isDragging) return;

    const deltaX = e.clientX - lastMouse.x;
    const deltaY = e.clientY - lastMouse.y;

    setRotation(prev => ({
      x: prev.x + deltaY * 0.01,
      y: prev.y + deltaX * 0.01
    }));

    setLastMouse({ x: e.clientX, y: e.clientY });
  };

  const handleMouseUp = () => {
    setIsDragging(false);
  };

  return (
    <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', padding: '20px' }}>
      <h2 style={{ marginBottom: '10px', fontFamily: 'Arial' }}>Water Molecule (H₂O)</h2>
      <canvas
        ref={canvasRef}
        width={400}
        height={400}
        onMouseDown={handleMouseDown}
        onMouseMove={handleMouseMove}
        onMouseUp={handleMouseUp}
        onMouseLeave={handleMouseUp}
        style={{
          border: '2px solid #333',
          borderRadius: '8px',
          cursor: isDragging ? 'grabbing' : 'grab',
          backgroundColor: '#f5f5f5'
        }}
      />
      <div style={{ marginTop: '10px', textAlign: 'center', fontFamily: 'Arial', fontSize: '14px' }}>
        <p><strong>Formula:</strong> H₂O</p>
        <p><strong>Molecular Weight:</strong> 18.015 u</p>
        <p><strong>Bond Angle:</strong> 104.5°</p>
        <p style={{ color: '#666', fontSize: '12px', marginTop: '10px' }}>
          Drag to rotate • Auto-rotates when released
        </p>
      </div>
    </div>
  );
};

export default WaterMolecule3D;