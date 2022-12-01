#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "ImGuizmoStl/ImCurveEditStl.h"
#include "ImGuizmoStl/ImGradientStl.h"
#include "ImGuizmoStl/ImZoomSliderStl.h"
#include "ImGuizmoStl/ImGuizmoStl.h"

#include <fplus/fplus.hpp>

namespace py = pybind11;
using namespace ImGuizmo;


// ============================================================================
// Hairy conversions between MatrixXX and numpy arrays, with shared memory
// (inspired by my previous work on cvnp (https://github.com/pthom/cvnp)
// ============================================================================

// ----------------------------------------------------------------------------
// Part 1 : Conversion utilities
// ----------------------------------------------------------------------------
namespace matrix_to_numpy
{

    template<typename MatrixXX>
    py::capsule make_capsule_matrix(const MatrixXX& m)
    {
        return py::capsule(new MatrixXX(m)
            , [](void *v) { delete reinterpret_cast<MatrixXX*>(v); }
        );
    }

    // we will transcribe a Matrix16 into a 4x4 matrix on python side
    std::vector<pybind11::ssize_t> matrix_shape(const Matrix16& m) { return {4, 4};}
    std::vector<pybind11::ssize_t> matrix_shape(const Matrix3& m) { return {3};}
    std::vector<pybind11::ssize_t> matrix_shape(const Matrix6& m) { return {6};}

    std::vector<pybind11::ssize_t> matrix_strides(const Matrix6& m) {
        return {sizeof(float)};
    }
    std::vector<pybind11::ssize_t> matrix_strides(const Matrix3& m) {
        return {sizeof(float)};
    }
    std::vector<pybind11::ssize_t> matrix_strides(const Matrix16& m) {
        return {
            sizeof(float) * 4,
            sizeof(float)
        };
    }


    template<typename MatrixXX>
    pybind11::array matrix_to_nparray(const MatrixXX& m, bool share_memory)
    {
        auto shape = matrix_shape(m);
        auto strides = matrix_strides(m);

        static std::string float_numpy_str = pybind11::format_descriptor<float>::format();
        static auto dtype_float = pybind11::dtype(float_numpy_str);

        std::cout << "matrix_to_nparray shape = " + fplus::show(shape) + "\n";
        if (share_memory)
            return pybind11::array(dtype_float, shape, strides, m.values, make_capsule_matrix(m));
        else
            return pybind11::array(dtype_float, shape, strides, m.values);
    }

    template<typename MatrixXX>
    MatrixXX nparray_to_matrix(pybind11::array& a)
    {
        MatrixXX r;

        // Check input array type and dimensions...
        if (a.dtype().kind() != pybind11::format_descriptor<float>::c)
            throw std::runtime_error("Only numpy arrays of type float are supported!");
        auto expected_shape = matrix_shape(r);
        if (a.ndim() != expected_shape.size())
            throw std::runtime_error("Bad shape ndim!");
        for (size_t i = 0; i < a.ndim(); ++i)
            if (a.shape()[i] != expected_shape[i])
                throw std::runtime_error("Bad shape!");

        // ...and then share its guts with MatrixXX
        r.use_external_values((float *)a.mutable_data(0));
        return r;
    }

}

// ----------------------------------------------------------------------------
// Part 1 : Type casters numpy.array <=> MatrixXX
// ----------------------------------------------------------------------------
namespace pybind11
{
    namespace detail
    {
        template<int N>
        struct type_caster<MatrixFixedSize<N>>
        {
        public:
        PYBIND11_TYPE_CASTER(MatrixFixedSize<N>, _("numpy.ndarray"));

            /**
             * Conversion part 1 (Python->C++):
             * Return false upon failure.
             * The second argument indicates whether implicit conversions should be applied.
             */
            bool load(handle src, bool)
            {
//                auto a = reinterpret_borrow<array>(src);
//                auto new_mat = cv::Mat(cvnp::nparray_to_mat(a));
//                value.Value = new_mat;
//                return true;

                auto a = reinterpret_borrow<array>(src);
                value =  matrix_to_numpy::nparray_to_matrix<MatrixFixedSize<N>>(a);
                return true;
            }

            /**
             * Conversion part 2 (C++ -> Python):
             * The second and third arguments are used to indicate the return value policy and parent object
             * (for ``return_value_policy::reference_internal``) and are generally
             * ignored by implicit casters.
             */
            static handle cast(const MatrixFixedSize<N> &m, return_value_policy, handle defval)
            {
                bool share_memory = true;
                auto a = matrix_to_numpy::matrix_to_nparray(m, true);
                return a.release();
            }
        };
    }
}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// <litgen_glue_code>  // Autogenerated code below! Do not edit!

namespace ImCurveEdit {
// helper type to enable overriding virtual methods in python
class Delegate_trampoline : public Delegate
{
public:
    using Delegate::Delegate;

    size_t GetCurveCount() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            size_t, // return type
            ImCurveEdit::Delegate, // parent class
            "get_curve_count", // function name (python)
            GetCurveCount // function name (c++)
        );
    }
    bool IsVisible(size_t param_0) override
    {
        PYBIND11_OVERRIDE_NAME(
            bool, // return type
            ImCurveEdit::Delegate, // parent class
            "is_visible", // function name (python)
            IsVisible, // function name (c++)
            param_0 // params
        );
    }
    ImCurveEdit::CurveType GetCurveType(size_t param_0) const override
    {
        PYBIND11_OVERRIDE_NAME(
            ImCurveEdit::CurveType, // return type
            ImCurveEdit::Delegate, // parent class
            "get_curve_type", // function name (python)
            GetCurveType, // function name (c++)
            param_0 // params
        );
    }
    ImVec2 & GetMin() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 &, // return type
            ImCurveEdit::Delegate, // parent class
            "get_min", // function name (python)
            GetMin // function name (c++)
        );
    }
    ImVec2 & GetMax() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 &, // return type
            ImCurveEdit::Delegate, // parent class
            "get_max", // function name (python)
            GetMax // function name (c++)
        );
    }
    size_t GetPointCount(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            size_t, // return type
            ImCurveEdit::Delegate, // parent class
            "get_point_count", // function name (python)
            GetPointCount, // function name (c++)
            curveIndex // params
        );
    }
    uint32_t GetCurveColor(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            uint32_t, // return type
            ImCurveEdit::Delegate, // parent class
            "get_curve_color", // function name (python)
            GetCurveColor, // function name (c++)
            curveIndex // params
        );
    }
    ImVec2 * GetPoints(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 *, // return type
            ImCurveEdit::Delegate, // parent class
            "get_points", // function name (python)
            GetPoints, // function name (c++)
            curveIndex // params
        );
    }
    int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            int, // return type
            ImCurveEdit::Delegate, // parent class
            "edit_point", // function name (python)
            EditPoint, // function name (c++)
            curveIndex, pointIndex, value // params
        );
    }
    void AddPoint(size_t curveIndex, ImVec2 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void, // return type
            ImCurveEdit::Delegate, // parent class
            "add_point", // function name (python)
            AddPoint, // function name (c++)
            curveIndex, value // params
        );
    }
    unsigned int GetBackgroundColor() override
    {
        PYBIND11_OVERRIDE_NAME(
            unsigned int, // return type
            ImCurveEdit::Delegate, // parent class
            "get_background_color", // function name (python)
            GetBackgroundColor // function name (c++)
        );
    }
    void BeginEdit(int param_0) override
    {
        PYBIND11_OVERRIDE_NAME(
            void, // return type
            ImCurveEdit::Delegate, // parent class
            "begin_edit", // function name (python)
            BeginEdit, // function name (c++)
            param_0 // params
        );
    }
    void EndEdit() override
    {
        PYBIND11_OVERRIDE_NAME(
            void, // return type
            ImCurveEdit::Delegate, // parent class
            "end_edit", // function name (python)
            EndEdit // function name (c++)
        );
    }
};
}  // namespace ImCurveEdit

namespace ImCurveEdit {
// helper type to enable overriding virtual methods in python
class DelegateStl_trampoline : public DelegateStl
{
public:
    using DelegateStl::DelegateStl;

    size_t GetPointCount(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_NAME(
            size_t, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_point_count", // function name (python)
            GetPointCount, // function name (c++)
            curveIndex // params
        );
    }
    ImVec2 * GetPoints(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_NAME(
            ImVec2 *, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_points", // function name (python)
            GetPoints, // function name (c++)
            curveIndex // params
        );
    }
    std::vector<ImVec2> & GetPointsList(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            std::vector<ImVec2> &, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_points_list", // function name (python)
            GetPointsList, // function name (c++)
            curveIndex // params
        );
    }
    size_t GetCurveCount() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            size_t, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_curve_count", // function name (python)
            GetCurveCount // function name (c++)
        );
    }
    bool IsVisible(size_t param_0) override
    {
        PYBIND11_OVERRIDE_NAME(
            bool, // return type
            ImCurveEdit::DelegateStl, // parent class
            "is_visible", // function name (python)
            IsVisible, // function name (c++)
            param_0 // params
        );
    }
    ImCurveEdit::CurveType GetCurveType(size_t param_0) const override
    {
        PYBIND11_OVERRIDE_NAME(
            ImCurveEdit::CurveType, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_curve_type", // function name (python)
            GetCurveType, // function name (c++)
            param_0 // params
        );
    }
    ImVec2 & GetMin() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 &, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_min", // function name (python)
            GetMin // function name (c++)
        );
    }
    ImVec2 & GetMax() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 &, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_max", // function name (python)
            GetMax // function name (c++)
        );
    }
    uint32_t GetCurveColor(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            uint32_t, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_curve_color", // function name (python)
            GetCurveColor, // function name (c++)
            curveIndex // params
        );
    }
    int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            int, // return type
            ImCurveEdit::DelegateStl, // parent class
            "edit_point", // function name (python)
            EditPoint, // function name (c++)
            curveIndex, pointIndex, value // params
        );
    }
    void AddPoint(size_t curveIndex, ImVec2 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void, // return type
            ImCurveEdit::DelegateStl, // parent class
            "add_point", // function name (python)
            AddPoint, // function name (c++)
            curveIndex, value // params
        );
    }
    unsigned int GetBackgroundColor() override
    {
        PYBIND11_OVERRIDE_NAME(
            unsigned int, // return type
            ImCurveEdit::DelegateStl, // parent class
            "get_background_color", // function name (python)
            GetBackgroundColor // function name (c++)
        );
    }
    void BeginEdit(int param_0) override
    {
        PYBIND11_OVERRIDE_NAME(
            void, // return type
            ImCurveEdit::DelegateStl, // parent class
            "begin_edit", // function name (python)
            BeginEdit, // function name (c++)
            param_0 // params
        );
    }
    void EndEdit() override
    {
        PYBIND11_OVERRIDE_NAME(
            void, // return type
            ImCurveEdit::DelegateStl, // parent class
            "end_edit", // function name (python)
            EndEdit // function name (c++)
        );
    }
};
}  // namespace ImCurveEdit

namespace ImGradient {
// helper type to enable overriding virtual methods in python
class Delegate_trampoline : public Delegate
{
public:
    using Delegate::Delegate;

    size_t GetPointCount() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            size_t, // return type
            ImGradient::Delegate, // parent class
            "get_point_count", // function name (python)
            GetPointCount // function name (c++)
        );
    }
    ImVec4 * GetPoints() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec4 *, // return type
            ImGradient::Delegate, // parent class
            "get_points", // function name (python)
            GetPoints // function name (c++)
        );
    }
    int EditPoint(int pointIndex, ImVec4 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            int, // return type
            ImGradient::Delegate, // parent class
            "edit_point", // function name (python)
            EditPoint, // function name (c++)
            pointIndex, value // params
        );
    }
    ImVec4 GetPoint(float t) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec4, // return type
            ImGradient::Delegate, // parent class
            "get_point", // function name (python)
            GetPoint, // function name (c++)
            t // params
        );
    }
    void AddPoint(ImVec4 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void, // return type
            ImGradient::Delegate, // parent class
            "add_point", // function name (python)
            AddPoint, // function name (c++)
            value // params
        );
    }
};
}  // namespace ImGradient

namespace ImGradient {
// helper type to enable overriding virtual methods in python
class DelegateStl_trampoline : public DelegateStl
{
public:
    using DelegateStl::DelegateStl;

    size_t GetPointCount() override
    {
        PYBIND11_OVERRIDE_NAME(
            size_t, // return type
            ImGradient::DelegateStl, // parent class
            "get_point_count", // function name (python)
            GetPointCount // function name (c++)
        );
    }
    ImVec4 * GetPoints() override
    {
        PYBIND11_OVERRIDE_NAME(
            ImVec4 *, // return type
            ImGradient::DelegateStl, // parent class
            "get_points", // function name (python)
            GetPoints // function name (c++)
        );
    }
    std::vector<ImVec4> & GetPointsList() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            std::vector<ImVec4> &, // return type
            ImGradient::DelegateStl, // parent class
            "get_points_list", // function name (python)
            GetPointsList // function name (c++)
        );
    }
    int EditPoint(int pointIndex, ImVec4 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            int, // return type
            ImGradient::DelegateStl, // parent class
            "edit_point", // function name (python)
            EditPoint, // function name (c++)
            pointIndex, value // params
        );
    }
    ImVec4 GetPoint(float t) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec4, // return type
            ImGradient::DelegateStl, // parent class
            "get_point", // function name (python)
            GetPoint, // function name (c++)
            t // params
        );
    }
    void AddPoint(ImVec4 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void, // return type
            ImGradient::DelegateStl, // parent class
            "add_point", // function name (python)
            AddPoint, // function name (c++)
            value // params
        );
    }
};
}  // namespace ImGradient

// </litgen_glue_code> // Autogenerated code end
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



void py_init_module_imguizmo(py::module& m)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // <litgen_pydef> // Autogenerated code below! Do not edit!
    ////////////////////    <generated_from:ImCurveEditStl.h>    ////////////////////

    { // <namespace ImCurveEdit>
        py::module_ pyNsImCurveEdit = m.def_submodule("ImCurveEdit", "");
        py::enum_<ImCurveEdit::CurveType>(pyNsImCurveEdit, "CurveType", py::arithmetic(), "")
            .value("curve_none", ImCurveEdit::CurveNone, "")
            .value("curve_discrete", ImCurveEdit::CurveDiscrete, "")
            .value("curve_linear", ImCurveEdit::CurveLinear, "")
            .value("curve_smooth", ImCurveEdit::CurveSmooth, "")
            .value("curve_bezier", ImCurveEdit::CurveBezier, "");


        auto pyNsImCurveEdit_ClassEditPoint =
            py::class_<ImCurveEdit::EditPoint>
                (pyNsImCurveEdit, "EditPoint", "")
            .def(py::init<>()) // implicit default constructor
            .def_readwrite("curve_index", &ImCurveEdit::EditPoint::curveIndex, "")
            .def_readwrite("point_index", &ImCurveEdit::EditPoint::pointIndex, "")
            .def("__lt__",
                &ImCurveEdit::EditPoint::operator<, py::arg("other"))
            ;


        auto pyNsImCurveEdit_ClassDelegate =
            py::class_<ImCurveEdit::Delegate, ImCurveEdit::Delegate_trampoline>
                (pyNsImCurveEdit, "Delegate", "")
            .def(py::init<>()) // implicit default constructor
            .def_readwrite("focused", &ImCurveEdit::Delegate::focused, "")
            .def("get_curve_count",
                &ImCurveEdit::Delegate::GetCurveCount)
            .def("is_visible",
                &ImCurveEdit::Delegate::IsVisible, py::arg("param_0"))
            .def("get_curve_type",
                &ImCurveEdit::Delegate::GetCurveType, py::arg("param_0"))
            .def("get_min",
                &ImCurveEdit::Delegate::GetMin)
            .def("get_max",
                &ImCurveEdit::Delegate::GetMax)
            .def("get_curve_color",
                &ImCurveEdit::Delegate::GetCurveColor, py::arg("curve_index"))
            .def("edit_point",
                &ImCurveEdit::Delegate::EditPoint, py::arg("curve_index"), py::arg("point_index"), py::arg("value"))
            .def("add_point",
                &ImCurveEdit::Delegate::AddPoint, py::arg("curve_index"), py::arg("value"))
            .def("get_background_color",
                &ImCurveEdit::Delegate::GetBackgroundColor)
            .def("begin_edit",
                &ImCurveEdit::Delegate::BeginEdit, py::arg("param_0"))
            .def("end_edit",
                &ImCurveEdit::Delegate::EndEdit)
            ;
        auto pyNsImCurveEdit_ClassDelegateStl =
            py::class_<ImCurveEdit::DelegateStl, ImCurveEdit::Delegate, ImCurveEdit::DelegateStl_trampoline>
                (pyNsImCurveEdit, "DelegateStl", "")
            .def(py::init<>()) // implicit default constructor
            .def("get_points_list",
                &ImCurveEdit::DelegateStl::GetPointsList, py::arg("curve_index"))
            ;


        pyNsImCurveEdit.def("edit_stl",
            ImCurveEdit::EditStl, py::arg("delegate"), py::arg("size"), py::arg("id"), py::arg("clipping_rect") = py::none());
    } // </namespace ImCurveEdit>
    ////////////////////    </generated_from:ImCurveEditStl.h>    ////////////////////


    ////////////////////    <generated_from:ImGradientStl.h>    ////////////////////

    { // <namespace ImGradient>
        py::module_ pyNsImGradient = m.def_submodule("ImGradient", "");
        auto pyNsImGradient_ClassDelegate =
            py::class_<ImGradient::Delegate, ImGradient::Delegate_trampoline>
                (pyNsImGradient, "Delegate", "")
            .def(py::init<>()) // implicit default constructor
            .def("edit_point",
                &ImGradient::Delegate::EditPoint, py::arg("point_index"), py::arg("value"))
            .def("get_point",
                &ImGradient::Delegate::GetPoint, py::arg("t"))
            .def("add_point",
                &ImGradient::Delegate::AddPoint, py::arg("value"))
            ;
        auto pyNsImGradient_ClassDelegateStl =
            py::class_<ImGradient::DelegateStl, ImGradient::Delegate, ImGradient::DelegateStl_trampoline>
                (pyNsImGradient, "DelegateStl", "")
            .def(py::init<>()) // implicit default constructor
            .def("get_points_list",
                &ImGradient::DelegateStl::GetPointsList)
            ;


        pyNsImGradient.def("edit_stl",
            ImGradient::EditStl, py::arg("delegate"), py::arg("size"));
    } // </namespace ImGradient>
    ////////////////////    </generated_from:ImGradientStl.h>    ////////////////////


    ////////////////////    <generated_from:ImZoomSliderStl.h>    ////////////////////

    { // <namespace ImZoomSlider>
        py::module_ pyNsImZoomSlider = m.def_submodule("ImZoomSlider", "");
        py::enum_<ImZoomSlider::ImGuiPopupFlags_>(pyNsImZoomSlider, "ImGuiZoomSliderFlags_", py::arithmetic(), "")
            .value("none", ImZoomSlider::ImGuiZoomSliderFlags_None, "")
            .value("vertical", ImZoomSlider::ImGuiZoomSliderFlags_Vertical, "")
            .value("no_anchors", ImZoomSlider::ImGuiZoomSliderFlags_NoAnchors, "")
            .value("no_middle_carets", ImZoomSlider::ImGuiZoomSliderFlags_NoMiddleCarets, "")
            .value("no_wheel", ImZoomSlider::ImGuiZoomSliderFlags_NoWheel, "");
        pyNsImZoomSlider.def("im_zoom_slider_stl",
            ImZoomSlider::ImZoomSliderStl, py::arg("lower"), py::arg("higher"), py::arg("view_lower"), py::arg("view_higher"), py::arg("wheel_ratio") = 0.01f, py::arg("flags") = ImZoomSlider::ImGuiZoomSliderFlags_None);
    } // </namespace ImZoomSlider>
    ////////////////////    </generated_from:ImZoomSliderStl.h>    ////////////////////


    ////////////////////    <generated_from:ImGuizmoStl.h>    ////////////////////

    { // <namespace ImGuizmo>
        py::module_ pyNsImGuizmo = m.def_submodule("ImGuizmo", "");
        pyNsImGuizmo.def("set_drawlist",
            ImGuizmo::SetDrawlist,
            py::arg("drawlist") = py::none(),
            " call inside your own window and before Manipulate() in order to draw gizmo to that window.\n Or pass a specific ImDrawList to draw to (e.g. ImGui::GetForegroundDrawList()).");

        pyNsImGuizmo.def("begin_frame",
            ImGuizmo::BeginFrame, "call BeginFrame right after ImGui_XXXX_NewFrame();");

        pyNsImGuizmo.def("set_im_gui_context",
            ImGuizmo::SetImGuiContext,
            py::arg("ctx"),
            " this is necessary because when imguizmo is compiled into a dll, and imgui into another\n globals are not shared between them.\n More details at https://stackoverflow.com/questions/19373061/what-happens-to-global-and-static-variables-in-a-shared-library-when-it-is-dynam\n expose method to set imgui context");

        pyNsImGuizmo.def("is_over",
            py::overload_cast<>(ImGuizmo::IsOver), "return True if mouse cursor is over any gizmo control (axis, plan or screen component)");

        pyNsImGuizmo.def("is_using",
            ImGuizmo::IsUsing, "return True if mouse IsOver or if the gizmo is in moving state");

        pyNsImGuizmo.def("enable",
            ImGuizmo::Enable,
            py::arg("enable"),
            " enable/disable the gizmo. Stay in the state until next call to Enable.\n gizmo is rendered with gray half transparent color when disabled");

        pyNsImGuizmo.def("set_rect",
            ImGuizmo::SetRect, py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"));

        pyNsImGuizmo.def("set_orthographic",
            ImGuizmo::SetOrthographic,
            py::arg("is_orthographic"),
            "default is False");


        py::enum_<ImGuizmo::OPERATION>(pyNsImGuizmo, "OPERATION", py::arithmetic(), " call it when you want a gizmo\n Needs view and projection matrices.\n matrix parameter is the source matrix (where will be gizmo be drawn) and might be transformed by the function. Return deltaMatrix is optional\n translation is applied in world space")
            .value("translate_x", ImGuizmo::TRANSLATE_X, "")
            .value("translate_y", ImGuizmo::TRANSLATE_Y, "")
            .value("translate_z", ImGuizmo::TRANSLATE_Z, "")
            .value("rotate_x", ImGuizmo::ROTATE_X, "")
            .value("rotate_y", ImGuizmo::ROTATE_Y, "")
            .value("rotate_z", ImGuizmo::ROTATE_Z, "")
            .value("rotate_screen", ImGuizmo::ROTATE_SCREEN, "")
            .value("scale_x", ImGuizmo::SCALE_X, "")
            .value("scale_y", ImGuizmo::SCALE_Y, "")
            .value("scale_z", ImGuizmo::SCALE_Z, "")
            .value("bounds", ImGuizmo::BOUNDS, "")
            .value("scale_xu", ImGuizmo::SCALE_XU, "")
            .value("scale_yu", ImGuizmo::SCALE_YU, "")
            .value("scale_zu", ImGuizmo::SCALE_ZU, "")
            .value("translate", ImGuizmo::TRANSLATE, "")
            .value("rotate", ImGuizmo::ROTATE, "")
            .value("scale", ImGuizmo::SCALE, "")
            .value("scaleu", ImGuizmo::SCALEU, "universal")
            .value("universal", ImGuizmo::UNIVERSAL, "");


        py::enum_<ImGuizmo::MODE>(pyNsImGuizmo, "MODE", py::arithmetic(), "")
            .value("local", ImGuizmo::LOCAL, "")
            .value("world", ImGuizmo::WORLD, "");


        pyNsImGuizmo.def("set_id",
            ImGuizmo::SetID, py::arg("id"));

        pyNsImGuizmo.def("is_over",
            py::overload_cast<ImGuizmo::OPERATION>(ImGuizmo::IsOver), py::arg("op"));

        pyNsImGuizmo.def("set_gizmo_size_clip_space",
            ImGuizmo::SetGizmoSizeClipSpace, py::arg("value"));

        pyNsImGuizmo.def("allow_axis_flip",
            ImGuizmo::AllowAxisFlip,
            py::arg("value"),
            " Allow axis to flip\n When True (default), the guizmo axis flip for better visibility\n When False, they always stay along the positive world/local axis");


        py::enum_<ImGuizmo::COLOR>(pyNsImGuizmo, "COLOR", py::arithmetic(), "")
            .value("direction_x", ImGuizmo::DIRECTION_X, "directionColor[0]")
            .value("direction_y", ImGuizmo::DIRECTION_Y, "directionColor[1]")
            .value("direction_z", ImGuizmo::DIRECTION_Z, "directionColor[2]")
            .value("plane_x", ImGuizmo::PLANE_X, "planeColor[0]")
            .value("plane_y", ImGuizmo::PLANE_Y, "planeColor[1]")
            .value("plane_z", ImGuizmo::PLANE_Z, "planeColor[2]")
            .value("selection", ImGuizmo::SELECTION, "selectionColor")
            .value("inactive", ImGuizmo::INACTIVE, "inactiveColor")
            .value("translation_line", ImGuizmo::TRANSLATION_LINE, "translationLineColor")
            .value("scale_line", ImGuizmo::SCALE_LINE, "")
            .value("rotation_using_border", ImGuizmo::ROTATION_USING_BORDER, "")
            .value("rotation_using_fill", ImGuizmo::ROTATION_USING_FILL, "")
            .value("hatched_axis_lines", ImGuizmo::HATCHED_AXIS_LINES, "")
            .value("text", ImGuizmo::TEXT, "")
            .value("text_shadow", ImGuizmo::TEXT_SHADOW, "")
            .value("count", ImGuizmo::COUNT, "");


        auto pyNsImGuizmo_ClassStyle =
            py::class_<ImGuizmo::Style>
                (pyNsImGuizmo, "Style", "")
            .def(py::init<>())
            .def_readwrite("translation_line_thickness", &ImGuizmo::Style::TranslationLineThickness, "Thickness of lines for translation gizmo")
            .def_readwrite("translation_line_arrow_size", &ImGuizmo::Style::TranslationLineArrowSize, "Size of arrow at the end of lines for translation gizmo")
            .def_readwrite("rotation_line_thickness", &ImGuizmo::Style::RotationLineThickness, "Thickness of lines for rotation gizmo")
            .def_readwrite("rotation_outer_line_thickness", &ImGuizmo::Style::RotationOuterLineThickness, "Thickness of line surrounding the rotation gizmo")
            .def_readwrite("scale_line_thickness", &ImGuizmo::Style::ScaleLineThickness, "Thickness of lines for scale gizmo")
            .def_readwrite("scale_line_circle_size", &ImGuizmo::Style::ScaleLineCircleSize, "Size of circle at the end of lines for scale gizmo")
            .def_readwrite("hatched_axis_line_thickness", &ImGuizmo::Style::HatchedAxisLineThickness, "Thickness of hatched axis lines")
            .def_readwrite("center_circle_size", &ImGuizmo::Style::CenterCircleSize, "Size of circle at the center of the translate/scale gizmo")
            ;


        pyNsImGuizmo.def("get_style",
            ImGuizmo::GetStyle);
        auto pyNsImGuizmo_ClassMatrixComponents =
            py::class_<ImGuizmo::MatrixComponents>
                (pyNsImGuizmo, "MatrixComponents", "")
            .def(py::init<>()) // implicit default constructor
            .def_readwrite("translation", &ImGuizmo::MatrixComponents::Translation, "")
            .def_readwrite("rotation", &ImGuizmo::MatrixComponents::Rotation, "")
            .def_readwrite("scale", &ImGuizmo::MatrixComponents::Scale, "")
            ;


        pyNsImGuizmo.def("my_16",
            ImGuizmo::my_16,
            "return_value_policy::reference",
            pybind11::return_value_policy::reference);

        pyNsImGuizmo.def("my_6",
            ImGuizmo::my_6,
            "return_value_policy::reference",
            pybind11::return_value_policy::reference);

        pyNsImGuizmo.def("my_3",
            ImGuizmo::my_3,
            "return_value_policy::reference",
            pybind11::return_value_policy::reference);

        pyNsImGuizmo.def("decompose_matrix_to_components",
            py::overload_cast<const Matrix16 &>(ImGuizmo::DecomposeMatrixToComponents), py::arg("matrix"));

        pyNsImGuizmo.def("recompose_matrix_from_components",
            py::overload_cast<const ImGuizmo::MatrixComponents &>(ImGuizmo::RecomposeMatrixFromComponents), py::arg("matrix_components"));

        pyNsImGuizmo.def("draw_cubes",
            py::overload_cast<const Matrix16 &, const Matrix16 &, const std::vector<Matrix16> &>(ImGuizmo::DrawCubes), py::arg("view"), py::arg("projection"), py::arg("matrices"));

        pyNsImGuizmo.def("draw_grid",
            py::overload_cast<const Matrix16 &, const Matrix16 &, const Matrix16 &, const float>(ImGuizmo::DrawGrid), py::arg("view"), py::arg("projection"), py::arg("matrix"), py::arg("grid_size"));

        pyNsImGuizmo.def("manipulate",
            py::overload_cast<const Matrix16 &, const Matrix16 &, ImGuizmo::OPERATION, ImGuizmo::MODE, Matrix16 &, std::optional<Matrix16>, std::optional<Matrix3>, std::optional<Matrix6>, std::optional<Matrix3>>(ImGuizmo::Manipulate), py::arg("view"), py::arg("projection"), py::arg("operation"), py::arg("mode"), py::arg("matrix"), py::arg("delta_matrix") = py::none(), py::arg("snap") = py::none(), py::arg("local_bounds") = py::none(), py::arg("bounds_snap") = py::none());

        pyNsImGuizmo.def("view_manipulate",
            py::overload_cast<Matrix16 &, float, ImVec2, ImVec2, ImU32>(ImGuizmo::ViewManipulate),
            py::arg("view"), py::arg("length"), py::arg("position"), py::arg("size"), py::arg("background_color"),
            "\n Please note that this cubeview is patented by Autodesk : https://patents.google.com/patent/US7782319B2/en\n It seems to be a defensive patent in the US. I don't think it will bring troubles using it as\n other software are using the same mechanics. But just in case, you are now warned!\n");

        pyNsImGuizmo.def("view_manipulate",
            py::overload_cast<Matrix16 &, const Matrix16 &, ImGuizmo::OPERATION, ImGuizmo::MODE, Matrix16 &, float, ImVec2, ImVec2, ImU32>(ImGuizmo::ViewManipulate),
            py::arg("view"), py::arg("projection"), py::arg("operation"), py::arg("mode"), py::arg("matrix"), py::arg("length"), py::arg("position"), py::arg("size"), py::arg("background_color"),
            "use this version if you did not call Manipulate before and you are just using ViewManipulate");
    } // </namespace ImGuizmo>
    ////////////////////    </generated_from:ImGuizmoStl.h>    ////////////////////

    // </litgen_pydef> // Autogenerated code end
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
