
#include "Renderer.h"


Renderer::Renderer()
{
    m_curShader = -1;
}

Renderer::~Renderer()
{

}

void Renderer::addShader(Shader* s)
{
    m_shaders.push_back(s);

    MatricesLocation mat;
    getMatrixUniLocs(s, mat);
    m_matricesUniLocs.push_back(mat);

    vector<DataPair*> v;
    m_allDataPairs.push_back(v);

    unordered_map<string, DataPair*> table;
    m_tables.push_back(table);
}




GLuint Renderer::GetUniformLocation(Shader* s, const char* name)
{
    unsigned int shaderID = s->getProgramId();

    GLuint location = glGetUniformLocation(shaderID, name);

    if(location == -1)
    {
        cout << "Error in Init_Shader with " << name << endl;
    }
    return location;
}


void Renderer::initDataPairUniLoc(DataPair* p, int pass, const char* name)
{
    p->uniLoc = getUniLoc(m_shaders[pass], name);
    m_allDataPairs[pass].push_back(p);
}

void Renderer::addDataPair(const char* name, int dataType)
{
    addDataPair(RENDER_PASS1, name, dataType);
}

void Renderer::addDataPair(int pass, const char* name, int dataType)
{
    DataPair* p;

    switch(dataType)
    {
        case DP_BOOL:
        {
            m_tables[pass][name] = new BoolDataPair(name);
            break;
        }
		case DP_INT:
        {
            m_tables[pass][name] = new IntDataPair(name);
            break;
        }
        case DP_FLOAT:
        {
            m_tables[pass][name] = new FloatDataPair(name);
            break;
        }
        case DP_VEC2:
        {
            m_tables[pass][name] = new Vec2DataPair(name);
            break;
        }
        case DP_VEC3:
        {
            m_tables[pass][name] = new Vec3DataPair(name);
            break;
        }
        case DP_VEC4:
        {
            m_tables[pass][name] = new Vec4DataPair(name);
            break;
        }
        case DP_MAT3:
        {
            m_tables[pass][name] = new Mat3DataPair(name);
            break;
        }
        case DP_MAT4:
        {
            m_tables[pass][name] = new Mat4DataPair(name);
            break;
        }
    }

    if(m_tables[pass].find(name) == m_tables[pass].end())
    {
        cout << "new DataPair Type" << endl;
        exit(1);
    }
    m_tables[pass][name]->uniLoc = getUniLoc(m_shaders[pass], name);
}




void Renderer::setData(int pass, const char* name, bool value)
{
    errorCheck(pass, name);
    m_tables[pass][name]->setValue(value);
}

void Renderer::setData(int pass, const char* name, int value)
{
    errorCheck(pass, name);
    m_tables[pass][name]->setValue(value);
}

void Renderer::setData(int pass, const char* name, int value, GLuint unit, GLuint textureId)
{
    errorCheck(pass, name);

    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, textureId);

    m_textureUnitStack.push(unit);
    m_tables[pass][name]->setValue(value);
}

void Renderer::setData(int pass, const char* name, float value)
{
    errorCheck(pass, name);
    m_tables[pass][name]->setValue(value);
}

void Renderer::setData(int pass, const char* name, glm::vec2 value)
{
    errorCheck(pass, name);
    m_tables[pass][name]->setValue(value);
}

void Renderer::setData(int pass, const char* name, glm::vec3 value)
{
    errorCheck(pass, name);
    m_tables[pass][name]->setValue(value);
}

void Renderer::setData(int pass, const char* name, glm::vec4 value)
{
    errorCheck(pass, name);
    m_tables[pass][name]->setValue(value);
}

void Renderer::setData(int pass, const char* name, glm::mat3 value)
{
    errorCheck(pass, name);
    m_tables[pass][name]->setValue(value);
}

void Renderer::setData(int pass, const char* name, glm::mat4 value)
{
    errorCheck(pass, name);
    m_tables[pass][name]->setValue(value);
}

void Renderer::errorCheck(int pass, const char* name)
{
    if(m_tables[pass].find(name) == m_tables[pass].end())
    {
        cout << "glUniLoc " << name << " is not found " << endl;
        exit(1);
    }
}





void Renderer::printTables()
{

    DataPair* dp = m_tables[1]["u_color"];

    for (int i=0; i<m_tables.size(); i++)
    {
        Utility::debug("Table ", i);

        for ( auto it = m_tables[i].begin(); it != m_tables[i].end(); ++it )
        {
            cout << " " << it->first;
            cout << ":" << it->second << endl;
        }

        Utility::debugLn(2);
    }
}



GLuint Renderer::getUniLoc(Shader* s, const char* name)
{
    return GetUniformLocation(s, name);
}

void Renderer::loadUniformLocations()
{
    loadUniformLocations(RENDER_PASS1);
}

void Renderer::loadUniformLocations(int pass)
{
    for ( auto local_it = m_tables[pass].begin(); local_it!= m_tables[pass].end(); ++local_it )
        (local_it->second)->setUniLoc();
}


void Renderer::loadUniformLocations(pipeline& p)
{
    loadUniformLocations(p, RENDER_PASS1);
}

void Renderer::loadUniformLocations(pipeline& p, int pass)
{
    loadUniformLocations(pass);

    if(pass != m_curShader)
    {
        Utility::debug("######### pass Not Matching");
    }

	if(!p.matricesReady)
	{
		p.modelViewMatrix=p.viewMatrix[p.viewMatrix.size()-1]*p.modelMatrix[p.modelMatrix.size()-1];
		p.modelViewProjectionMatrix=p.projectionMatrix[p.projectionMatrix.size()-1]*
                                    p.viewMatrix[p.viewMatrix.size()-1]*
                                    p.modelMatrix[p.modelMatrix.size()-1];
		p.normalMatrix=glm::mat3(p.modelViewMatrix);
	}

	glUniformMatrix4fv(m_matricesUniLocs[pass].ModelMatrix,1,GL_FALSE,&p.modelMatrix[p.modelMatrix.size()-1][0][0]);
    glUniformMatrix4fv(m_matricesUniLocs[pass].ViewMatrix,1,GL_FALSE,&p.viewMatrix[p.viewMatrix.size()-1][0][0]);
	glUniformMatrix4fv(m_matricesUniLocs[pass].ModelviewMatrix,1,GL_FALSE,&p.modelViewMatrix[0][0]);
	glUniformMatrix4fv(m_matricesUniLocs[pass].ModelviewProjection,1,GL_FALSE,&p.modelViewProjectionMatrix[0][0]);
	glUniformMatrix3fv(m_matricesUniLocs[pass].NormalMatrix,1,GL_FALSE,&p.normalMatrix[0][0]);
}



void Renderer::enableShader()
{
    enableShader(RENDER_PASS1);
}

void Renderer::enableShader(int RenderPassID)
{
    m_shaders[RenderPassID]->useShader();
    m_curShader = RenderPassID;
}


void Renderer::disableShader()
{
    disableShader(RENDER_PASS1);
}

void Renderer::disableShader(int RenderPassID)
{
    m_shaders[RenderPassID]->delShader();
    m_curShader = -1;
    while(!m_textureUnitStack.empty())
    {
        glActiveTexture(m_textureUnitStack.top());
        glBindTexture(GL_TEXTURE_2D, 0);
        m_textureUnitStack.pop();
    }
}

void Renderer::setUniformLocation(GLuint location, bool value)
{
    glUniform1i(location, value);
}

void Renderer::setUniformLocation(GLuint location, int value)
{
    glUniform1i(location, value);
}

void Renderer::setUniformLocation(GLuint location, float value)
{
    glUniform1f(location, value);
}

void Renderer::setUniformLocation(GLuint location, float x, float y)
{
    glUniform2f(location, x, y);
}

void Renderer::setUniformLocation(GLuint location, glm::vec2 value)
{
    glUniform2f(location, value.x, value.y);
}

void Renderer::setUniformLocation(GLuint location, glm::vec3 value)
{
    glUniform3f(location, value.x, value.y, value.z);
}

/// http://glm.g-truc.net/0.9.2/api/a00132_source.html
/// source code of glm::vec4, you see the union
void Renderer::setUniformLocation(GLuint location, glm::vec4 value)
{
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Renderer::setUniformLocation(GLuint location, glm::mat3 value)
{
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void Renderer::setUniformLocation(GLuint location, glm::mat4 value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Renderer::setUniformLocationTranspose(GLuint location, glm::mat4 value)
{
    glUniformMatrix4fv(location, 1, GL_TRUE, &value[0][0]);
}

void Renderer::render()
{

}


void Renderer::getAllMatrixUniLocs()
{
    for(int i=0; i<m_numShaders; i++)
        getMatrixUniLocs(m_shaders[i], m_matricesUniLocs[i]);

    //    getMatrixUniLocs(m_shaders[i], Matrices_UniLoc[i]);

}


bool Renderer::getMatrixUniLocs(Shader* s, MatricesLocation& Mat)
{
    Mat.ModelviewProjection = getUniLoc( s, "u_modelViewProjMat");
    Mat.ModelviewMatrix     = getUniLoc( s, "u_modelViewMat");
    Mat.ViewMatrix          = getUniLoc( s, "u_viewMat");
    Mat.ProjectionMatrix    = getUniLoc( s, "u_projMat");
    Mat.ModelMatrix         = getUniLoc( s, "u_modelMat");
    Mat.ViewNoRotateMatrix  = getUniLoc( s, "u_viewNoRotateMat");
    Mat.NormalMatrix        = getUniLoc( s, "u_normalMat");

    if(Mat.ModelviewProjection == -1 ||
       Mat.ModelviewMatrix == -1 ||
       Mat.ViewMatrix == -1 ||
       Mat.ProjectionMatrix == -1 ||
       Mat.ModelMatrix == -1 ||
       Mat.ViewNoRotateMatrix == -1 ||
       Mat.NormalMatrix)
    {
        cout << "Error in Init_Shader" << endl;
        return false;
    }
    return true;
}





void Renderer::setAllDataPairUniLocs(int pass)
{
    int size = m_allDataPairs[pass].size();
    for(int i=0; i<size; i++)
        m_allDataPairs[pass][i]->setUniLoc();
 //       setDataPairUniLoc( m_allDataPairs[pass][i]->uniLoc, m_allDataPairs[pass][i]->value);
}




void Renderer::setDataPairUniLoc(BoolDataPair& dp)
{
    setUniformLocation(dp.uniLoc, dp.value);
}

void Renderer::setDataPairUniLoc(IntDataPair& dp)
{
    setUniformLocation(dp.uniLoc, dp.value);
}

void Renderer::setDataPairUniLoc(FloatDataPair& dp)
{
    setUniformLocation(dp.uniLoc, dp.value);
}

void Renderer::setDataPairUniLoc(Vec2DataPair& dp)
{
    setUniformLocation(dp.uniLoc, dp.value.x, dp.value.y);
}

void Renderer::setDataPairUniLoc(Vec3DataPair& dp)
{
    setUniformLocation(dp.uniLoc, dp.value);
}

void Renderer::setDataPairUniLoc(Vec4DataPair& dp)
{
    setUniformLocation(dp.uniLoc, dp.value);
}

void Renderer::setDataPairUniLoc(Mat3DataPair& dp)
{
    setUniformLocation(dp.uniLoc, dp.value);
}

void Renderer::setDataPairUniLoc(Mat4DataPair& dp)
{
    setUniformLocation(dp.uniLoc, dp.value);
}


