/***************************************************************
 * Name:
 * Purpose:   Header for OpenGL rendering window
 * Author:    Keith Coffman (dcoffm5261@gmail.com)
 * Created:   2018-06-22
 **************************************************************/

#ifndef glpane_H
#define glpane_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <vector>
#include "wxGLMain.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


class wxGLFrame;
class Shader;

class BasicGLPane : public wxGLCanvas
{
    wxGLContext*	m_context;
    wxGLFrame*      parent;

public:
	BasicGLPane(wxFrame* frameParent, wxGLFrame* realParent);
	virtual ~BasicGLPane();

    //bool opt_realtime = false;
    bool opt_zmap = true;
    bool opt_fill = false;
    bool opt_stream = false;
    bool opt_grad = false;

    float vertices1[12] = {0.0, 0.0, 0.0, 1.,
                            1.0, 0.0, 0.0, 0.,
                            0.5, 1.0, 0.0, 0.};

    unsigned int indices[3] = {0,1,2};

    // Color map
    double cmapR[64] = {0.2422,0.250390476190476,0.257771428571429,0.264728571428571,0.270647619047619,0.275114285714286,0.2783,0.280333333333333,0.281338095238095,0.281014285714286,0.279466666666667,0.275971428571429,0.269914285714286,0.260242857142857,0.244033333333333,0.220642857142857,0.196333333333333,0.183404761904762,0.178642857142857,0.176438095238095,0.168742857142857,0.154,0.146028571428571,0.138023809523810,0.124814285714286,0.111252380952381,0.0952095238095238,0.0688714285714285,0.0296666666666667,0.00357142857142858,0.00665714285714287,0.0433285714285715,0.0963952380952380,0.140771428571429,0.1717,0.193766666666667,0.216085714285714,0.246957142857143,0.290614285714286,0.340642857142857,0.3909,0.445628571428572,0.5044,0.561561904761905,0.617395238095238,0.671985714285714,0.7242,0.773833333333333,0.820314285714286,0.863433333333333,0.903542857142857,0.939257142857143,0.972757142857143,0.995647619047619,0.996985714285714,0.995204761904762,0.9892,0.978628571428571,0.967647619047619,0.961009523809524,0.959671428571429,0.962795238095238,0.969114285714286,0.9769};
    double cmapG[64] = {0.1504,0.164995238095238,0.181780952380952,0.197757142857143,0.214676190476190,0.234238095238095,0.255871428571429,0.278233333333333,0.300595238095238,0.322757142857143,0.344671428571429,0.366680952380952,0.3892,0.412328571428571,0.435833333333333,0.460257142857143,0.484719047619048,0.507371428571429,0.528857142857143,0.549904761904762,0.570261904761905,0.5902,0.609119047619048,0.627628571428572,0.645928571428571,0.6635,0.679828571428571,0.694771428571429,0.708166666666667,0.720266666666667,0.731214285714286,0.741095238095238,0.75,0.7584,0.766961904761905,0.775766666666667,0.7843,0.791795238095238,0.797290476190476,0.8008,0.802871428571429,0.802419047619048,0.7993,0.794233333333333,0.787619047619048,0.779271428571429,0.769842857142857,0.759804761904762,0.749814285714286,0.7406,0.733028571428571,0.728785714285714,0.729771428571429,0.743371428571429,0.765857142857143,0.789252380952381,0.813566666666667,0.838628571428572,0.8639,0.889019047619048,0.913457142857143,0.937338095238095,0.960628571428571,0.9839};
    double cmapB[64] = {0.6603,0.707614285714286,0.751138095238095,0.795214285714286,0.836371428571429,0.870985714285714,0.899071428571429,0.9221,0.941376190476191,0.957885714285714,0.971676190476191,0.982904761904762,0.9906,0.995157142857143,0.998833333333333,0.997285714285714,0.989152380952381,0.979795238095238,0.968157142857143,0.952019047619048,0.935871428571429,0.9218,0.907857142857143,0.897290476190476,0.888342857142857,0.876314285714286,0.859780952380952,0.839357142857143,0.816333333333333,0.7917,0.766014285714286,0.739409523809524,0.712038095238095,0.684157142857143,0.655442857142857,0.6251,0.5923,0.556742857142857,0.518828571428572,0.478857142857143,0.435447619047619,0.390919047619048,0.348,0.304480952380953,0.261238095238095,0.2227,0.191028571428571,0.164609523809524,0.153528571428571,0.159633333333333,0.177414285714286,0.209957142857143,0.239442857142857,0.237147619047619,0.219942857142857,0.202761904761905,0.188533333333333,0.176557142857143,0.164290476190476,0.153676190476191,0.142257142857143,0.126509523809524,0.106361904761905,0.0805};

    static const unsigned int numObjects  = 10;

    // indices used by openGL for keeping track of objects
    unsigned int VBOs[numObjects]; // Vertex buffet objects
    unsigned int EBOs[numObjects]; // Element buffer objects
    unsigned int VAOs[numObjects]; // Vertex array objects
    unsigned int TEXs[numObjects]; // Texture objects
    unsigned int VAO_=0; // currently bound VAO (location in VAOs array)

    // Shader objects
    Shader shader1; // Color mapping shader (for plotting function results)
    Shader shader2; // Fixed color shader (for plotting boundary conditions and pre-simulation meshes)

    // Rendering matrices
    glm::vec3 cameraPos, cameraPos_start, cameraTarget, cameraTarget_start, cameraDirection, cameraRight, cameraUp, cameraUp_abs;
    glm::mat4 model, view, projection;
    float R0=3., R=3.; // radius of sphere around which view orbits target
    int zoomInc = 0; // zoom level

	int getWidth();
	int getHeight();
	int n=3; // nTri*3

	// EVENT CALLBACKS
	// window
	void render(wxPaintEvent& evt);
	void doBuffer(int bufferIndex,bool makeCurrent);
	void resized(wxSizeEvent& evt);
	void updatePlotOps();
	void resetView();

	// mouse & keyboard
	void mouseMoved(wxMouseEvent& event);
	void mouseLDown(wxMouseEvent& event);
	void mouseLUp(wxMouseEvent& event);
	void mouseRDown(wxMouseEvent& event);
	void mouseRUp(wxMouseEvent& event);
	void mouseRDClick(wxMouseEvent& event);
	void mouseMDown(wxMouseEvent& event);
	void mouseMUp(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);


	wxCoord dragX, dragY; // records initial position before dragging to compute how much to change angle/translation
	int dragType = 0; // What button is being dragged (0=none, 1=left, 2=right, 3=middle)

	DECLARE_EVENT_TABLE()
};
#endif
