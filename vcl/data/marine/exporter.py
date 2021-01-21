import json


with open('marine_anims_core.json', 'r') as f:
    array = json.load(f)

skeleton = array['geometries'][0]['data']['bones']

f = open('skeleton_connectivity','w')
for k,e in enumerate(skeleton):
    f.write(str(k)+" "+str(e['parent'])+" "+e['name']+"\n")
f.close()

f = open('skeleton_geometry_local','w')
for k,e in enumerate(skeleton):
    f.write( str(e['pos'][0])+" "+str(e['pos'][1])+" "+str(e['pos'][2])+" "+str(e['rotq'][0])+" "+str(e['rotq'][1])+" "+str(e['rotq'][2])+" "+str(e['rotq'][3])+'\n')
f.close()

anim = array['geometries'][0]['data']['animations']

f = open('skeleton_animation_run','w')

for k in range(len(anim[1]['hierarchy'])):
    number_of_keys = len(anim[1]['hierarchy'][k]['keys'])
    f.write(str(number_of_keys)+'\n')
    for kt,e in enumerate(anim[1]['hierarchy'][k]['keys']):
        p = e['pos']
        q = e['rot']
        t = e['time']
        p_str = str(p[0])+' '+str(p[1])+' '+str(p[2])
        q_str = str(q[0])+' '+str(q[1])+' '+str(q[2])+' '+str(q[3])
        f.write(str(t)+' '+p_str+' '+q_str+'\n')
f.close()



# #export mesh
# f = open('mesh.obj','w')
# vertices = array['geometries'][0]['data']['vertices']
# for k in range(int(len(vertices)/3)):
#     f.write('v '+str(vertices[3*k+0])+' '+str(vertices[3*k+1])+' '+str(vertices[3*k+2])+'\n')

# faces = array['geometries'][0]['data']['faces']
# print(len(faces))
# for k in range(int(len(faces)/3)):
#     f.write('f '+str(faces[3*k+0]+1)+' '+str(faces[3*k+1]+1)+' '+str(faces[3*k+2]+1)+'\n')
    

# f.close()

weights = array['geometries'][0]['data']['skinWeights']
bone_idx = array['geometries'][0]['data']['skinIndices']

f = open('skinning_data','w')
f.write('2\n')
for k in range(len(weights)):
    f.write(str(bone_idx[k])+' '+str(weights[k])+'\n')
f.close()