// file      : libstudxml/serializer.hxx -*- C++ -*-
// license   : MIT; see accompanying LICENSE file

#ifndef LIBSTUDXML_SERIALIZER_HXX
#define LIBSTUDXML_SERIALIZER_HXX

#include <libstudxml/details/pre.hxx>

#include <string>
#include <ostream>
#include <cstddef> // std::size_t

#include <libstudxml/details/genx/genx.h>

#include <libstudxml/forward.hxx>
#include <libstudxml/qname.hxx>
#include <libstudxml/exception.hxx>

#include <libstudxml/details/config.hxx>
#include <libstudxml/details/export.hxx>

namespace xml
{
  class serialization: public exception
  {
  public:
    virtual
    ~serialization () noexcept {}

    serialization (const std::string& name, const std::string& description);
    serialization (const serializer& s, const std::string& description);

    const std::string&
    name () const {return name_;}

    const std::string&
    description () const {return description_;}

    virtual const char*
    what () const noexcept {return what_.c_str ();}

  private:
    LIBSTUDXML_EXPORT void
    init ();

  private:
    std::string name_;
    std::string description_;
    std::string what_;
  };

  class LIBSTUDXML_EXPORT serializer
  {
  public:
    typedef xml::qname qname_type;

    // Serialize to std::ostream. Output name is used in diagnostics to
    // identify the document being serialized. The indentation argument
    // specifies the number of indentation spaces that should be used for
    // pretty-printing. If 0 is passed, no pretty-printing is performed.
    //
    // If stream exceptions are enabled then std::ios_base::failure
    // exception is used to report io errors (badbit and failbit).
    // Otherwise, those are reported as the serialization exception.
    //
    serializer (std::ostream&,
                const std::string& output_name,
                unsigned short indentation = 2);

    const std::string&
    output_name () const {return oname_;}

    ~serializer ();

  private:
    serializer (const serializer&);
    serializer& operator= (const serializer&);

    // Serialization functions.
    //
  public:

    // Elements.
    //
    void
    start_element (const qname_type& qname);

    void
    start_element (const std::string& name);

    void
    start_element (const std::string& ns, const std::string& name);

    void
    end_element ();

    // "Checked" end element. That is, it checks that the element
    // you think you are ending matches the current one.
    //
    void
    end_element (const qname_type& qname);

    void
    end_element (const std::string& name);

    void
    end_element (const std::string& ns, const std::string& name);


    // Helpers for serializing elements with simple content. The first two
    // functions assume that start_element() has already been called. The
    // other two serialize the complete element, from start to end.
    //
    void
    element (const std::string& value);

    template <typename T>
    void
    element (const T& value);

    void
    element (const std::string& name, const std::string& value);

    template <typename T>
    void
    element (const std::string& name, const T& value);

    void
    element (const qname_type& qname, const std::string& value);

    template <typename T>
    void
    element (const qname_type& qname, const T& value);

    void
    element (const std::string& namespace_,
             const std::string& name,
             const std::string& value);

    template <typename T>
    void
    element (const std::string& namespace_,
             const std::string& name,
             const T& value);

    // Attributes.
    //
    void
    start_attribute (const qname_type& qname);

    void
    start_attribute (const std::string& name);

    void
    start_attribute (const std::string& ns, const std::string& name);

    void
    end_attribute ();

    // "Checked" end attribute. That is, it checks that the attribute
    // you think you are ending matches the current one.
    //
    void
    end_attribute (const qname_type& qname);

    void
    end_attribute (const std::string& name);

    void
    end_attribute (const std::string& ns, const std::string& name);


    void
    attribute (const qname_type& qname, const std::string& value);

    template <typename T>
    void
    attribute (const qname_type& qname, const T& value);

    void
    attribute (const std::string& name, const std::string& value);

    template <typename T>
    void
    attribute (const std::string& name, const T& value);

    void
    attribute (const std::string& ns,
               const std::string& name,
               const std::string& value);

    template <typename T>
    void
    attribute (const std::string& ns,
               const std::string& name,
               const T& value);

    // Characters.
    //
    void
    characters (const std::string& value);

    template <typename T>
    void
    characters (const T& value);

    // Namespaces declaration. If prefix is empty, then the default
    // namespace is declared. If both prefix and namespace are empty,
    // then the default namespace declaration is cleared (xmlns="").
    //
    // This function should be called after start_element().
    //
    void
    namespace_decl (const std::string& ns, const std::string& prefix);

    // XML declaration. If encoding or standalone are not specified,
    // then these attributes are omitted from the output.
    //
    void
    xml_decl (const std::string& version = "1.0",
              const std::string& encoding = "UTF-8",
              const std::string& standalone = "");

    // DOCTYPE declaration. If encoding or standalone are not specified,
    // then these attributes are omitted from the output.
    //
    void
    doctype_decl (const std::string& root_element,
                  const std::string& public_id = "",
                  const std::string& system_id = "",
                  const std::string& internal_subset = "");

    // Utility functions.
    //
  public:
    // Return true if there is a mapping. In this case, prefix contains
    // the mapped prefix.
    //
    bool
    lookup_namespace_prefix (const std::string& ns, std::string& prefix) const;

    // Return the current element, that is, the latest element for which
    // start_element() but not end_element() have been called.
    //
    qname
    current_element () const;

    // Return the current attribute, that is, the latest attribute for
    // which start_attribute() but not end_attribute() have been called.
    //
    qname
    current_attribute () const;

    // Suspend/resume indentation.
    //
  public:

    // Indentation can be suspended only inside an element and, unless
    // explicitly resumed, it will remain suspended until the end of
    // that element. You should only explicitly resume indentation at
    // the element nesting level of suspension. If indentation is already
    // suspended at an outer nesting level, then subsequent calls to
    // suspend/resume are ignored. The indentation_suspended() function
    // can be used to check if indentation is currently suspended. If it
    // is not, then this function returns 0. Otherwise, it returns the
    // level at which pretty-printing was suspended, with root element
    // being level 1.
    //
    void
    suspend_indentation ();

    void
    resume_indentation ();

    std::size_t
    indentation_suspended () const;

  private:
    void
    handle_error (genxStatus) const;

  private:
    std::ostream& os_;
    std::ostream::iostate os_state_; // Original exception state.
    const std::string oname_;

    genxWriter s_;
    genxSender sender_;
    std::size_t depth_;
  };

  // Stream-like interface for serializer. If the passed argument is
  // callable with the serializer as its argument, then this function
  // (object) is called with the passed serializer. Otherwise, the
  // argument is passed to the serializer's characters() function.
  //
  template <typename T>
  serializer&
  operator<< (serializer&, const T& value);
}

#include <libstudxml/serializer.ixx>

#include <libstudxml/details/post.hxx>

#endif // LIBSTUDXML_SERIALIZER_HXX
