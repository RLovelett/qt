/**
 * Test the ability to define macros that get processed by the moc preprocessor.
 *
 * This is incredible useful when extending a series of QObject derived classes
 * by the same subset of properties. Without this feature, all the properties
 * must be maintained in each class header file with the drawback of generating
 * a lot of duplicated code.
 *
 * The macro-expansion only works for macros without parameters - still I see
 * no requirement for function macros here.
 */
#ifdef Q_MOC_RUN
#define MACRO_TEST_PROPERTIES \
    Q_PROPERTY(int macroIntProperty READ macroIntProperty) \
    Q_PROPERTY(QString macroStringProperty READ macroStringProperty WRITE setMacroStringProperty)
#define MACRO_TEST_CLASSINFO \
    Q_CLASSINFO("macrotest", "true")
#else
#define MACRO_TEST_PROPERTIES
#define MACRO_TEST_CLASSINFO
#endif

class MacroTestClass {
 public:
   MacroTestClass () : m_macroStringProperty ("tEsTvAlUe") {}
   virtual ~MacroTestClass () {}

   int macroIntProperty () {return 0;}
   QString macroStringProperty () const {return m_macroStringProperty;}
   void setMacroStringProperty (const QString & s) {m_macroStringProperty=s;}

 private:
   QString m_macroStringProperty;
};
