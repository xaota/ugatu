package svgparser;

import java.io.FileWriter;
import java.io.IOException;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/**
 * @author Rinat
 */
public class Main {
  /**
   * Возвращает "русскую" форму записи числительного
   * @param k - число для расчёта
   * @param form1 - запись при k = 1
   * @param form2 - запись при k = 2
   * @param form5 - запись при k = 5
   * @return "k числительное"
   */
  public static String rusform(int k, String form1, String form2, String form5){ return rusform(k,form1,form2,form5,true); }
  /**
   * Возвращает "русскую" форму записи числительного
   * @param k - число для расчёта
   * @param form1 - запись при k = 1
   * @param form2 - запись при k = 2
   * @param form5 - запись при k = 5
   * @param x - флаг включения k в результат
   * @return "k числительное" или "числительное"
   */
  public static String rusform(int k, String form1, String form2, String form5, boolean x){
  int n = Math.abs(k) % 100, n1 = n % 10;
  if ((n > 20 || n < 10) && n1>1 && n1 < 5) return x ? k + " " + form2 : form2;
  if (n != 11 && n1 == 1) return x ? k + " " + form1 : form1;
  return x ? k + " " + form5 : form5;
 }
  /**
   * Главная функция программы
   * @param args - аргументы коммандной строки
   */
  public static void main(String[] args) {
  SvgParser svgParser = new SvgParser("clouds");
  svgParser.process();
 }
}

/**
 * Класс, ыполняющий действия с SVG документом
 * @author Rinat
 */
class SvgParser {
 private String fileName;

 /**
  * Конструктор объекта класса
  * @param fileName
  */
 public SvgParser(String fileName) { setFileName(fileName); }
 
 /**
  * Обработка SVG документа (по заданию)
  */
 public void process(){
  String input = fileName+".svg", output = fileName+"_out.svg";
  String colorRed = "red", colorBlue = "blue";
  try {
   System.out.println("Считываемем документ... ("+input+")");
   Document doc = readXmlToDOMDocument(input);
   System.out.println("Документ ("+input+") успешно считан!");
   NodeList circles = elementsGet(doc, "circle");
   centerCircles(doc, circles);
   NodeList rects = elementsGet(doc, "rect");
   reFillRects(rects, colorBlue);
   NodeList polylines = elementsGet(doc, "polyline");
   NodeList paths = elementsGet(doc, "path");
   reStrokePaths(paths, colorRed);
   System.out.println("Сохраняем документ... ("+output+")");
   saveDOMDocumentToXML(doc, output);
   System.out.println("Документ ("+output+") Успешно сохранён!");
  } catch (Exception ex) {
   System.err.println(ex.getMessage());
  }
 }
 
 /**
  * Добавляет центры к окружностям. (Создаёт новые маленькие сразу после уже имеющихся)
  * @param doc - DOM структура в памяти
  * @param circles - Список узлоа типа circle
  */
 private void centerCircles(Document doc, NodeList circles){
  int length = circles.getLength();
  for (int i = 0; i < length; i++) {
   Element circle = (Element)(circles.item(i));
   Element center = insertAfter(doc, circle, "circle");
   ++length; // Общее количество элементов увеличилось
   ++i; // После этого кода i указывает на номер только что созданного элемента
   center.setAttribute("cx", circle.getAttribute("cx"));
   center.setAttribute("cy", circle.getAttribute("cy"));
   center.setAttribute("r",  "3");
   center.setAttribute("style", "fill:red;stroke:none");
   System.out.println(serialize(circle));
  }
 }
 /**
  * Перекрашивает прямоугольники в цвет color. Предыдущий style не сохраняется
  * @param rects - Список узлов прямоугольников
  * @param color - новый цвет
  */
 private void reFillRects(NodeList rects, String color){
  int length = rects.getLength();
  for (int i = 0; i < length; i++){
   Element rect = (Element)(rects.item(i));
   rect.setAttribute("style", "fill:"+color);
  }
 }
 /**
  * Перекрашивает пути в цвет color. Предыдущий style не сохраняется
  * @param paths - список узлов путей
  * @param color - новый цвет
  */
 private void reStrokePaths(NodeList paths, String color){
  int length = paths.getLength();
  for (int i = 0; i < length; i++){
   Element path = (Element)(paths.item(i));
   path.setAttribute("style", "stroke:"+color);
  }
 }

 
 
 /**
  * Считывает SVG документ из файла в память
  * @param fileName - имя файла для чтения
  * @return Document - Объект DOM
  * @throws Exception - возможные ошибки будут направлены в вызывающий метож
  */
 private Document readXmlToDOMDocument(String fileName) throws Exception {
  DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
  //  используется шаблон проектирования "Абстрактная фабрика"
  //  DocumentBuilderFactory.newInstance() порождает объект класса-наследникв от абстрактного класса DocumentBuilderFactory
  //  Объект какого типа будет порожден, зависит от конфигурации JDK
  //  Сейчас используется парсер Xerces и класс com.sun.org.apache.xerces.internal.jaxp.DocumentBuilderFactoryImpl

  dbf.setNamespaceAware(false); // не поддерживать пространства имен
  dbf.setValidating(true); // DTD проверка
  dbf.setCoalescing(true); // склеивать CDATA с ближайшим текстовым блоком
  dbf.setIgnoringElementContentWhitespace(true);

  DocumentBuilder db;
  try {
   db = DocumentBuilderFactory.newInstance().newDocumentBuilder();
   try {
    return db.parse(fileName); // читаем документ
   } catch (IOException ex) {
    throw new Exception("Ошибка чтения документа");
   }
  } catch (ParserConfigurationException ex) {
    throw new Exception("Ошибка создания Document Builder!");
  }
 }
 /**
  * Сохраняет DOM документ в XML файл.
  * @param doc - DOM структура документа в памяти
  * @param fileName - имя выходного файла
  * @throws Exception - возникающие исключения будут направлены в вызвавший метод
  */
 private void saveDOMDocumentToXML(Document doc, String fileName) throws Exception {
  StreamResult sr = new StreamResult(new FileWriter(fileName));
  //StreamResult sr2 = new StreamResult(System.out);

  DOMSource domSource = new DOMSource(doc);
  //  DOMSource domSource = new DOMSource(i2);
  // в newTransformer() можно было бы передать xslt - преобразование
  Transformer tr;
  try {
   tr = TransformerFactory.newInstance().newTransformer();
   // настройки "преобразования"
   tr.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
   tr.setOutputProperty(OutputKeys.INDENT, "yes"); // отступы

   tr.transform(domSource, sr ); // в файл
   //tr.transform(domSource, sr2 ); // на экран
  } catch (TransformerException ex) {
   throw new Exception("Ошибка преобразования документа!");
  }
 }
 /**
  * Возвращат список узлов документа по имени тега (getElementsByTagName)
  * @param doc - DOM структура в памяти
  * @param nodeName - имя тега искомых узлов
  * @return список узлов определённого типа
  */
 private NodeList findNodes(Document doc, String nodeName){
  Element root = (Element)(doc.getDocumentElement()); // Node -> Element
  NodeList nodes = root.getElementsByTagName(nodeName);
  return nodes;
 }
 /**
  * Создаёт узел с именем nodeName, в DOM структуре doc, не привязывая ни к какому узлу
  * @param doc - DOM структура в памяти
  * @param nodeName - имя создаваемого узла
  * @return Созданный узел
  */
 private Element createNode(Document doc, String nodeName){
  Element node = doc.createElement(nodeName);
  return node;
 }
 /**
  * Вставляет узел Node сразу после узла reference
  * @param reference - Узел после которого стоит добавить элемент
  * @param node - добавляемый элемент
  * @return добавленный элемент
  */
 private Element insertAfter(Element reference, Element node){
  Node parent = reference.getParentNode();
  parent.insertBefore(node, reference.getNextSibling());
  return node;
 }
 /**
  * Создаёт узел с именем nodeName и вставляет его после узла reference в DOM структуре doc
  * @param doc - DOM структура в памяти
  * @param reference - узел, после которого будет создан и вставлен новый узел
  * @param nodeName - имя создаваемого и вставляемого узла
  * @return созданный и вставленный узел
  */
 private Element insertAfter(Document doc, Element reference, String nodeName){ 
   return insertAfter(reference, createNode(doc, nodeName)); 
 }
 
 /**
  * Возвращает (рекурсивно) XML-запись узла со всем содержимым (только для тегов)
  * @param node
  * @return 
  */
 private String serialize(Node node){
  if(node.getNodeType() != Node.ELEMENT_NODE) return "";
  String name = node.getNodeName(),
  s = "<"+name;
  if(node.hasAttributes()){
   NamedNodeMap attributes = node.getAttributes();
   int count = attributes.getLength();
   for (int i = 0; i < count; i++) {
    Node attribute = attributes.item(i);
    String attributeName = attribute.getNodeName();
    String attributeValue = attribute.getNodeValue();
    s += " "+attributeName+"=\""+attributeValue+"\"";
   }
  }
  if(node.hasChildNodes()){
   s += ">";
   NodeList nodes = node.getChildNodes();
   int length = nodes.getLength();
   for (int i = 0; i < length; i++) {
    s += serialize(nodes.item(i));
   }
   s+="</"+name+">";
  }else{
   s+= " />";
  }
  return s;
 }
 
 /**
  * Выполняет поиск определённых узлов, сообщает об этом на консоль и возвращает найденные узлы
  * @param doc - DOM структура в памяти
  * @param element - Название искомого узла
  * @return Список найдённых узлов
  */
 private NodeList elementsGet(Document doc, String element){
  System.out.println("Ищем элементы <"+element+">...");
  NodeList elements = findNodes(doc, element);
  System.out.println("Найдено " + Main.rusform(elements.getLength(), "элемент", "элемента", "элементов")+"  <"+element+">");
  return elements;
  }

  /**
   * @return the fileName
   */
  public String getFileName() {
    return fileName;
  }

  /**
   * @param fileName the fileName to set
   */
  public final void setFileName(String fileName) {
    this.fileName = fileName;
  }
}
