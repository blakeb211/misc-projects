//  Loads blender 3d model file. INCOMPLETE. Sat Oct 12 06:21:32 PM EDT 2024

int load_model(std::string inputfile, vector<vec3>& out_verts, vector<vec2>&out_uv, vector<vec3>& out_norms)  {

  Attrib attrib;
  ShapeVec shapes;
  MaterialVec materials;

  std::string warn;
  std::string err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              inputfile.c_str());

  if (!warn.empty()) {
    std::cout << warn << std::endl;
  }
  if (!err.empty()) {
    std::cerr << err << std::endl;
  }
  if (!ret) {
    exit(1);
  }

  std::cout << "Model: " << inputfile << "\n";
  size_t index_offset = 0;
  float bmin[3];
  float bmax[3];
  for (int i = 0; i < 3; i++) bmin[i] = std::numeric_limits<float>::max();
  for (int i = 0; i < 3; i++) bmax[i] = std::numeric_limits<float>::min();

  const auto & shape = shapes[0];
  std::cout << "triangles " << shape.mesh.indices.size() / 3.0 << std::endl;
  std::cout << "vertices " << attrib.vertices.size() / 3.0 << std::endl;
  // NOTE: normals are part I least understand and we should prob just calculate ourselves 
  std::cout << "normals " << attrib.normals.size() / 3.0 << std::endl; 
  std::cout << "tex_coords " << attrib.texcoords.size() / 2.0 << std::endl;


  for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {
    // Get the three indexes of the face (all faces are triangular)
    tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
    tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
    tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];
    // each idx has a texcoord_index
    auto idx0_u = attrib.texcoords[2 * idx0.texcoord_index + 0];
    auto idx0_v = attrib.texcoords[2 * idx0.texcoord_index + 1];
    auto idx1_u = attrib.texcoords[2 * idx1.texcoord_index + 0];
    auto idx1_v = attrib.texcoords[2 * idx1.texcoord_index + 1];
    auto idx2_u = attrib.texcoords[2 * idx2.texcoord_index + 0];
    auto idx2_v = attrib.texcoords[2 * idx2.texcoord_index + 1];

    // Get the three vertex indexes and coordinates
    int vi[3];      // indexes
    float v[3][3];  // coordinates
    float n[3][3];  // normals
      
    n[0][0] = attrib.normals[3 * idx0.normal_index + 0];
    n[0][1] = attrib.normals[3 * idx0.normal_index + 1];
    n[0][2] = attrib.normals[3 * idx0.normal_index + 2];
    n[1][0] = attrib.normals[3 * idx1.normal_index + 0];
    n[1][1] = attrib.normals[3 * idx1.normal_index + 1];
    n[1][2] = attrib.normals[3 * idx1.normal_index + 2];
    n[2][0] = attrib.normals[3 * idx2.normal_index + 0];
    n[2][1] = attrib.normals[3 * idx2.normal_index + 1];
    n[2][2] = attrib.normals[3 * idx2.normal_index + 2];

    for (int k = 0; k < 3; k++) {
      vi[0] = idx0.vertex_index;
      vi[1] = idx1.vertex_index;
      vi[2] = idx2.vertex_index;
      assert(vi[0] >= 0);
      assert(vi[1] >= 0);
      assert(vi[2] >= 0);

      v[0][k] = attrib.vertices[3 * vi[0] + k];
      v[1][k] = attrib.vertices[3 * vi[1] + k];
      v[2][k] = attrib.vertices[3 * vi[2] + k];
      
      // calc min and max coord to sanity 
      // check against the tinyobjloader viewer app
      bmin[k] = std::min(v[0][k], bmin[k]);
      bmin[k] = std::min(v[1][k], bmin[k]);
      bmin[k] = std::min(v[2][k], bmin[k]);
      bmax[k] = std::max(v[0][k], bmax[k]);
      bmax[k] = std::max(v[1][k], bmax[k]);
      bmax[k] = std::max(v[2][k], bmax[k]);
    }
   
    out_verts.push_back(vec3(v[0][0],v[0][1],v[0][2]));
    out_verts.push_back(vec3(v[1][0],v[1][1],v[1][2]));
    out_verts.push_back(vec3(v[2][0],v[2][1],v[2][2]));
    
    out_uv.push_back(vec2(idx0_u,idx0_v));
    out_uv.push_back(vec2(idx1_u,idx1_v));
    out_uv.push_back(vec2(idx2_u,idx2_v));
    
    out_norms.push_back(vec3(n[0][0],n[0][1],n[0][2]));
    out_norms.push_back(vec3(n[1][0],n[1][1],n[1][2]));
    out_norms.push_back(vec3(n[2][0],n[2][1],n[2][2]));

  }
  std::cout << "bminx " << bmin[0] << " bminy " << bmin[1] << " bminz " << bmin[2] << "\n";
  std::cout << "bmaxx " << bmax[0] << " bmaxy " << bmax[1] << " bmaxz " << bmax[2] << "\n"; 

  return 0;
}
