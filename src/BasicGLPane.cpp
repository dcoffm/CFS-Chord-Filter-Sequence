/***************************************************************
 * Name:      BasicGLPane.cpp
 * Purpose:   Window for handling OpenGL graphics
 * Author:    Keith Coffman (dcoffm5261@gmail.com)
 * Created:   2018-06-22
 **************************************************************/
#include "BasicGLPane.h"

BasicGLPane::BasicGLPane(wxFrame* frameParent, wxGLFrame* realParent) :
    wxGLCanvas(frameParent, wxID_ANY, NULL, wxDefaultPosition, wxSize(1000,563), wxFULL_REPAINT_ON_RESIZE)
{
    parent = realParent;
    wxGLContextAttrs cxtAttrs;
    cxtAttrs.CoreProfile().OGLVersion(4, 4).Robust().EndList();

	m_context = new wxGLContext(this,NULL,&cxtAttrs);
	this->SetCurrent(*m_context);
	if(glewInit()!=GLEW_OK){
        wxMessageBox("Glew isn't ok.");
	}

	glViewport(0,0,getWidth(),getHeight());
	glEnable(GL_DEPTH_TEST);

	// Make the projection matrix
    projection = glm::perspective(glm::radians(45.0f), float(getWidth())/float(getHeight()), 0.1f, 20.0f); // Also consider glk::ortho projection?

    shader2.loadSrc("shader_fixed.vs", "shader_fixed.fs");
    shader1.loadSrc("shader_map.vs", "shader_map.fs");
    shader1.use();  // Use before setting uniforms!
    shader1.setMat4("projection",projection);
    shader2.use();
    shader2.setMat4("projection",projection);

    resetView();

    // Set up buffers and objects
    glGenVertexArrays(numObjects, VAOs);
    glGenBuffers(numObjects, VBOs);
    glGenBuffers(numObjects, EBOs);
    glGenTextures(numObjects,TEXs);

    doBuffer(0,true);

    glBindTexture(GL_TEXTURE_1D, TEXs[0]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    float colormap[192];
    for(int i=0;i<64;i++){
        colormap[3*i+0] = float(cmapR[i]);
        colormap[3*i+1] = float(cmapG[i]);
        colormap[3*i+2] = float(cmapB[i]);
    }
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGB32F,64,0,GL_RGB,GL_FLOAT,colormap);
    glUniform1i(glGetUniformLocation(shader1.ID, "colormap"), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(VAOs[0]); // Default to VAO[0] plot state
    VAO_ = 0;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draw in wireframe polygons.
}

BasicGLPane::~BasicGLPane(){
    glDeleteVertexArrays(numObjects, VAOs);
    glDeleteBuffers(numObjects, VBOs);
    glDeleteBuffers(numObjects, EBOs);
	delete m_context;
}

void BasicGLPane::resized(wxSizeEvent& evt){
//	wxGLCanvas::OnSize(evt);
    Refresh();
}

int BasicGLPane::getWidth(){    return GetSize().x;}
int BasicGLPane::getHeight(){    return GetSize().y;}

void BasicGLPane::render( wxPaintEvent& evt ){
    if(!IsShown()) return;
    SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClearColor(0.14f, 0.21f, 0.21f, 1.0f);
    glClearDepth(1.0f);	// Depth Buffer Setup
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(VAO_==0){
        shader1.use(); shader1.setMat4("view",view);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    }
    if(VAO_==1){
        shader2.use(); shader2.setMat4("view",view);
        glDrawArrays(GL_LINES,0,2*parent->nLines);
    }
    if(VAO_==2){
        shader1.use(); shader1.setMat4("view",view);
        if(parent->realtime){
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*parent->N, parent->nodeArray.data(), GL_DYNAMIC_DRAW);
        }
        glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
    }
    if(VAO_==3){
        shader1.use(); shader1.setMat4("view",view);
        glDrawArrays(GL_TRIANGLES,0,n);
    }

    glFlush();
    SwapBuffers();
}

void BasicGLPane::doBuffer(int buffInd,bool makeCurrent){
    parent->bufferMutex->Lock();
    //wxLogDebug("Enter buffer, %i",buffInd);
    glBindVertexArray(VAOs[buffInd]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[buffInd]);
    if(buffInd==0){ // a triangle, since there's nothing else to show
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[buffInd]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0); // 3 spatial coordinates
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(3*sizeof(float))); // function value
    }
    if(buffInd==1){ // Pre-simulation mesh display
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*6*parent->nLines, parent->lineArray.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);                  // 3 spatial coordinates
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float))); // 3 color values
    }
    if(buffInd==2){ // Post-simulation value display u(x)
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*parent->N, parent->nodeArray.data(), GL_DYNAMIC_DRAW); // or GL_STATIC_DRAW
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);                  // 3 spatial coordinates
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(3*sizeof(float))); // 1 function value
        n = 3*parent->nTri;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[buffInd]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*n, parent->triArray.data(), GL_STATIC_DRAW);
    }
    if(buffInd==3){ // Post-simulation gradient display
        n = 3*parent->nTri;
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*n, parent->gradArray.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);                  // 3 spatial coordinates
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(3*sizeof(float))); // 1 function value
    }
    glEnableVertexAttribArray(0); glEnableVertexAttribArray(1);
    if(makeCurrent)
        VAO_ = buffInd;
    else
        glBindVertexArray(VAOs[VAO_]);
    //wxLogDebug("Exit  buffer, %i",buffInd);
    parent->bufferMutex->Unlock();
}
void BasicGLPane::updatePlotOps(){
    if(opt_fill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Add stuff to handle streamlines and gradient plot
    if(opt_stream)
        wxMessageBox("This option hasn't been implemented yet");

    // Need to have a different way of handling this.
    if(opt_grad && VAO_==2){
        VAO_=3;
        glBindVertexArray(VAOs[VAO_]);
    }
    if(!opt_grad && VAO_==3){
        VAO_=2;
        glBindVertexArray(VAOs[VAO_]);
    }

    Refresh();
}

// Mouse & key interactions
void BasicGLPane::mouseMoved(wxMouseEvent& event){
    float deltaX = event.GetX() - dragX;
    float deltaY = event.GetY() - dragY;

    if(dragType==2 && opt_zmap){
        // Update camera position
        cameraPos = cameraPos_start + (-cameraRight*deltaX + cameraUp*deltaY)*R/100.f;
        cameraDirection = glm::normalize(cameraPos - cameraTarget);
        cameraPos = cameraTarget + cameraDirection * R;
        view = glm::lookAt(cameraPos,cameraTarget,cameraUp_abs);
        if(sqrt(deltaX*deltaX+deltaY*deltaY)>10.){
            cameraPos_start = cameraPos;
            cameraRight = glm::normalize(glm::cross(cameraUp_abs, cameraDirection));
            cameraUp = glm::cross(cameraDirection, cameraRight);
            dragX = event.GetX();
            dragY = event.GetY();
        }
    }
    if(dragType==3){
        cameraTarget = cameraTarget_start + (-cameraRight*deltaX + cameraUp*deltaY)*R/500.f;
        cameraPos = cameraPos_start + (-cameraRight*deltaX + cameraUp*deltaY)*R/500.f;
        view = glm::lookAt(cameraPos,cameraTarget,cameraUp);
        if(sqrt(deltaX*deltaX+deltaY*deltaY)>10.){
            cameraPos_start = cameraPos;
            cameraTarget_start = cameraTarget;
            dragX = event.GetX();
            dragY = event.GetY();
        }
    }
    Refresh();
}
void BasicGLPane::mouseLDown(wxMouseEvent& event){
    SetFocus();
    if(dragType==0){
        dragType = 1;
        dragX = event.GetX();
        dragY = event.GetY();
        CaptureMouse();
    }
}
void BasicGLPane::mouseLUp(wxMouseEvent& event){
    if(dragType==1){
        dragType = 0;
        ReleaseMouse();
    }
}
void BasicGLPane::mouseRDown(wxMouseEvent& event){
    SetFocus();
    if(dragType==0){
        dragType = 2;
        dragX = event.GetX();
        dragY = event.GetY();
        cameraPos_start = cameraPos; // Small movements will be relative to the initial position for precision
        CaptureMouse();
    }
}
void BasicGLPane::mouseRDClick(wxMouseEvent& event){

}
void BasicGLPane::mouseRUp(wxMouseEvent& event){
    if(dragType==2){
        dragType = 0;
        ReleaseMouse();
    }
}
void BasicGLPane::mouseMDown(wxMouseEvent& event){
    SetFocus();
    if(dragType==0){
        dragType = 3;
        dragX = event.GetX();
        dragY = event.GetY();
        CaptureMouse();
        cameraPos_start = cameraPos;
        cameraTarget_start = cameraTarget;
    }
}
void BasicGLPane::mouseMUp(wxMouseEvent& event){
    if(dragType==3){
        dragType = 0;
        ReleaseMouse();
    }
}

void BasicGLPane::mouseWheelMoved(wxMouseEvent& event){
    zoomInc -= event.GetWheelRotation()/event.GetWheelDelta();
    R = R0*pow(1.2,double(zoomInc));
    cameraPos = cameraTarget + cameraDirection * R;
    view = glm::lookAt(cameraPos,cameraTarget,cameraUp);
    Refresh();
}

void BasicGLPane::mouseLeftWindow(wxMouseEvent& event){ }
void BasicGLPane::keyPressed(wxKeyEvent& event){
    //if(event.GetKeyCode()==WXK_CONTROL_C){
        resetView();
    //}
}
void BasicGLPane::resetView(){
    model = glm::translate(glm::mat4(1.0), glm::vec3(-(parent->xmax+parent->xmin)/2,-(parent->ymax+parent->ymin)/2,0.f)); // Centers the simulation around world origin
    if(opt_zmap){ // 3D
        R = R0 = 3.; zoomInc = 0;
        cameraPos = glm::vec3(2.0f, -2.0f, 1.0f);
        cameraUp_abs = glm::vec3(0.,0.,1.);
        model[2][2] = 1;
    }
    else{ // 2D
        R = R0 = 1.5; zoomInc = 0;
        cameraPos = glm::vec3(0.0f, 0.0f, R);
        cameraUp_abs = glm::vec3(0.,1.,0.);
        model[2][2] = 0;
    }

    cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraDirection = glm::normalize(cameraPos - cameraTarget);
    cameraRight = glm::normalize(glm::cross(cameraUp_abs, cameraDirection));
    cameraUp = glm::cross(cameraDirection, cameraRight);
    view = glm::lookAt(cameraPos,cameraTarget,cameraUp);
    shader1.use();
    shader1.setMat4("model",model);
    shader2.use();
    shader2.setMat4("model",model);
    Refresh();
}
void BasicGLPane::keyReleased(wxKeyEvent& event){ }
