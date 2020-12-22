import hou
import math

sceneRoot = hou.node('obj')

# simulation starting values
numIterations = 25
dt = 0.1
gravity = hou.Vector3(0, -9.8, 0)
nodes = []
velocity = hou.Vector3(0.0, 0.2, 0.0)
angVelocity = hou.Vector3(0.5, 0.0, 0.0)
orientation = hou.Quaternion(0.2, 0.0, 0.0, 0.0)
adaptationStrength = 0.9
surfaces = []
transforms = []

class Node:
  def __init__(self, count, parent=None, position=hou.Vector3(0, 0, 0)):
    self.parent = parent
    self.position = position
    self.velocity = velocity
    self.angVelocity = angVelocity
    self.orientation = orientation
    if parent is not None:
      self.group = [parent, self]
    else:
      self.group = [self]
    self.count = count
    self.geo = None

  def step(self):
    # update orientation
    accNorm = self.parent.angVelocity.length()
    accY = math.sin((accNorm * dt) / 2)
    accX = math.cos((accNorm * dt) / 2)

    eulerQuat = hou.Quaternion(accX, 
                              self.parent.angVelocity[0] * accY, 
                              self.parent.angVelocity[1] * accY, 
                              self.parent.angVelocity[2] * accY)
    
    qp = eulerQuat * self.parent.orientation
    self.orientation = qp

    # update acceleration
    orientQuat = qp * self.parent.orientation.inverse()

    angleAxis = orientQuat.extractAngleAxis()
    acc = angleAxis[0] * angleAxis[1]
    self.angVelocity = acc

    # update position
    self.position = (self.parent.position + self.parent.velocity * dt + 
                    (self.parent.angVelocity + gravity) * dt * dt) / 2.0

    # update velocity
    self.velocity = (self.position - self.parent.position) / dt

    # surface adaptation
    if not len(surfaces):
      for surface in hou.node('/obj').selectedItems():
        surfaces.append(surface.displayNode().geometry())
        transform = hou.hmath.buildTranslate(surface.evalParm('tx'), 
                                             surface.evalParm('ty'), 
                                             surface.evalParm('tz'))
        rotate = hou.hmath.buildRotate(surface.evalParm('rx'), 
                                       surface.evalParm('ry'), 
                                       surface.evalParm('rz'))
        scale = hou.hmath.buildScale(surface.evalParm('sx'), 
                                     surface.evalParm('sy'), 
                                     surface.evalParm('sz'))
        transforms.append(scale * rotate * transform)
    for i in range(len(surfaces)):
      point = surfaces[i].nearestPoint(self.position * transforms[i].inverted(), 
                                       max_radius=3.0)
      if point is not None:
        surfaceVec = point.position() * transforms[i] - self.position
        axis = surfaceVec.cross(self.velocity)
        angle = surfaceVec.dot(self.velocity) * adaptationStrength * dt
        self.orientation = hou.Quaternion(angle, axis)
      bounds = surfaces[i].boundingBox()
      minBounds = bounds.minvec() * transforms[i]
      maxBounds = bounds.maxvec() * transforms[i]
      xmin = minBounds[0]
      ymin = minBounds[1]
      zmin = minBounds[2]
      xmax = maxBounds[0]
      ymax = maxBounds[1]
      zmax = maxBounds[2]
      bounds.setTo((xmin, ymin, zmin, xmax, ymax, zmax))
      if bounds.contains(self.position):
        parentDir = (self.parent.position - self.position).normalized()
        while (bounds.contains(self.position)):
          self.position += 0.1 * parentDir

    self.geo = self.createGeom()

  def createGeom(self):

    if self.parent == None:
      geo = sceneRoot.createNode('geo')
      geo.setName('SEED')
    else:
      geo = sceneRoot.createNode('geo')
      geo.setName('PARTICLE_{}'.format(self.count))
      
      # create line 
      lineGeo = sceneRoot.createNode('geo')
      lineGeo.setName('LINE_{}_{}'.format(self.count, self.parent.count))
      line = lineGeo.createNode('line')
      line.setName('LINE_{}'.format(self.count))
      linePosX = line.parm('originx')
      linePosY = line.parm('originy')
      linePosZ = line.parm('originz')
      linePosX.set(self.parent.position[0])
      linePosY.set(self.parent.position[1])
      linePosZ.set(self.parent.position[2])
      lineLength = line.parm('dist')
      lineLength.set((self.parent.position - self.position).length())
      lineDirX = line.parm('dirx')
      lineDirY = line.parm('diry')
      lineDirZ = line.parm('dirz')
      lineDir = (self.position - self.parent.position).normalized()
      lineDirX.set(lineDir[0])
      lineDirY.set(lineDir[1])
      lineDirZ.set(lineDir[2])
      
      
    sphere = geo.createNode('sphere')
    sphere.setName('SPHERE_{}'.format(self.count))

    sphereRadX = sphere.parm('radx')
    sphereRadY = sphere.parm('rady')
    sphereRadZ = sphere.parm('radz')
    sphereRadX.set(0.2)
    sphereRadY.set(0.2)
    sphereRadZ.set(0.2)
    
    spherePosX = sphere.parm('tx')
    spherePosY = sphere.parm('ty')
    spherePosZ = sphere.parm('tz')
    spherePosX.set(self.position[0])
    spherePosY.set(self.position[1])
    spherePosZ.set(self.position[2])
    
    return geo
    
 
for i in range(numIterations):
  if i < 1: 
    currNode = Node(count=i)
    currNode.geo = currNode.createGeom()
  else:
    currNode = Node(i, nodes[i - 1], nodes[i - 1].position)
    currNode.step()
    
  nodes.append(currNode)
  
for i in range(len(nodes)):
  nodes[i].group.append(nodes[i + 1:])
