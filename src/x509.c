#include <stdio.h>
#include <string.h>

#include "x509.h"

//
// For details on TLV (Type, Length, Value) in the ASN1 DER format see
// https://www.itu.int/ITU-T/studygroups/com17/languages/X.690-0207.pdf
//
// GetTLV takes a pointer to a TLV structure and returns the class, encoding,
// type, length, and value. It does not support types and lengths greater than
// 32 bits and also does not support indefinite forms. Normal x509 should not
// need any of that.
//
// p   - pointer to the beginning of a TLV structure
// pc  - receives the class
// pe  - receives the enoding
// pt  - receives the type
// pl  - receives the length
// ppv - receives a pointer to the value
//
// Returns a pointer to the next sibling TLV structure on success,
// otherwise NULL
//
uint8_t* GetTLV(uint8_t* p, uint8_t* pc, uint8_t* pe, uint32_t* pt, uint32_t* pl, uint8_t** ppv) {
  int i = 0, c = 0;
  *pc = ((*p) & 0xC0) >> 6; *pe = ((*p) & 0x20) >> 5;
  *pt = 0; *pl = 0; *ppv = NULL;

  if (((*p) & 0x1F) == 0x1F) {
    // long form type
    do { c++; p++; *pt = ((*pt) << 7) | ((*p) & 0x7F);
    } while (((*p) & 0x80) == 0x80);
    if (c > 4) return NULL;
  } else {
    // short form type
    *pt = ((*p) & 0x1F);
  }
  p++; // advance to length

  if (((*p) & 0x80) == 0x80) {
    // long or indefinite form length
    c = ((*p) & 0x7F);
    if (c > 4 || c < 1) return NULL;
    for (i = 0; i < c; i++) {
      p++; *pl = ((*pl) << 8) | (*p);
    }
  } else {
    // short form length
    *pl = ((*p) & 0x7F);
  }
  // advance to value, set value, advance to next
  p++; *ppv = p; p += *pl;
  return p;
}

#ifdef SEANK_DEBUG
#define DBG_OUT(s) printf("%s(c=%d e=%d t=%u l=%u)\n", s, c, e, t, l);
#define DUMP_OID() \
  printf("OID = "); \
  for(uint32_t i = 0; i < l; i++) printf("0x%X ", pv[i]); \
  printf("\n");
#define DUMP_VALUE() \
  printf("Value = "); \
  for(uint32_t i = 0; i < l; i++) printf("%c", pv[i]); \
  printf("\n");
#else
#define DBG_OUT(s)
#define DUMP_OID()
#define DUMP_VALUE()
#endif

#define TLV(s, p, pn, expect) \
  pn = GetTLV(p, &c, &e, &t, &l, &pv); \
  DBG_OUT(s); \
  if (pn==NULL || !(expect)) \
    return -1; \

#define EXPECT(class, encoding, type) (c==class && e==encoding && t==type)

//
// For details about x509 certificates see https://tools.ietf.org/html/rfc5280
//
// getCommonName takes a pointer to the beginning of an x509 certificate in
// ASN1 DER format and walks the TLV structures to find the common name
//
// Returns a pointer to a static buffer containing the common name on success,
// otherwise NULL
//
int getCommonName(uint8_t* p, char* pr, uint32_t rl) {
  uint8_t *pn, c, e, *pv; uint32_t t, l;
  // Certificate  ::=  SEQUENCE  {
  //      tbsCertificate       TBSCertificate,
  //      signatureAlgorithm   AlgorithmIdentifier,
  //      signatureValue       BIT STRING  }
  TLV("Certificate", p, pn, \
    EXPECT(CLASS_UNIVERSAL, ENCODING_CONSTRUCTED, TYPE_SEQUENCE));
    // TBSCertificate  ::=  SEQUENCE  {
    //      version         [0]  EXPLICIT Version DEFAULT v1,
    //      serialNumber         CertificateSerialNumber,
    //      signature            AlgorithmIdentifier,
    //      issuer               Name,
    //      validity             Validity,
    //      subject              Name,
    TLV("TBSCertificate", pv, pn,
      EXPECT(CLASS_UNIVERSAL, ENCODING_CONSTRUCTED, TYPE_SEQUENCE));
      TLV("version", pv, pn,
        EXPECT(CLASS_CONTEXT_SPECIFIC, ENCODING_CONSTRUCTED, TYPE_BOOLEAN));
      TLV("serialNumber", pn, pn,
        EXPECT(CLASS_UNIVERSAL, ENCODING_PRIMITIVE, TYPE_INTEGER));
      TLV("signature", pn, pn,
        EXPECT(CLASS_UNIVERSAL, ENCODING_CONSTRUCTED, TYPE_SEQUENCE));
      TLV("issuer", pn, pn,
        EXPECT(CLASS_UNIVERSAL, ENCODING_CONSTRUCTED, TYPE_SEQUENCE));
      TLV("validity", pn, pn,
        EXPECT(CLASS_UNIVERSAL, ENCODING_CONSTRUCTED, TYPE_SEQUENCE));
      TLV("subject", pn, pn,
        EXPECT(CLASS_UNIVERSAL, ENCODING_CONSTRUCTED, TYPE_SEQUENCE));
        // Name ::= CHOICE { -- only one possibility for now --
        //   rdnSequence  RDNSequence }
        //
        // RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
        //
        // RelativeDistinguishedName ::=
        //  SET SIZE (1..MAX) OF AttributeTypeAndValue

        // subject contains a sequence or ordered sets of oid value pairs

        // save the next sibling so we know where to stop and
        // update pn so the loop below can process both the first
        // inner child and all subsequent siblings
        uint8_t* pe = pn; pn = pv;
        while(pn < pe) {
          TLV("rdnSequence", pn, pn,
            EXPECT(CLASS_UNIVERSAL, ENCODING_CONSTRUCTED, TYPE_SET));
            // AttributeTypeAndValue ::= SEQUENCE {
            //  type     AttributeType,
            //  value    AttributeValue }
            //
            // AttributeType ::= OBJECT IDENTIFIER
            //
            // AttributeValue ::= ANY -- DEFINED BY AttributeType
            //
            // DirectoryString ::= CHOICE {
            //       teletexString           TeletexString (SIZE (1..MAX)),
            //       printableString         PrintableString (SIZE (1..MAX)),
            //       universalString         UniversalString (SIZE (1..MAX)),
            //       utf8String              UTF8String (SIZE (1..MAX)),
            //       bmpString               BMPString (SIZE (1..MAX)) }
            uint8_t* pattr;
            TLV("AttributeTypeAndValue", pv, pattr,
              EXPECT(CLASS_UNIVERSAL, ENCODING_CONSTRUCTED, TYPE_SEQUENCE));
              uint8_t* ptv;
              TLV("type", pv, ptv,
                EXPECT(CLASS_UNIVERSAL, ENCODING_PRIMITIVE, TYPE_OBJECT_ID));
              DUMP_OID()
              // common name is oid 1.21.4.3
              int cn = (l==3 && pv[0]==85 && pv[1]==4 && pv[2]==3) ? 1 : 0;
              TLV("value", ptv, ptv,
                EXPECT(CLASS_UNIVERSAL, ENCODING_PRIMITIVE, t));
              if (t == TYPE_UTF8_STRING || t == TYPE_PRINTABLE_STRING) {
                DUMP_VALUE()
                if (cn && l <= rl) {
                  strncpy(pr, (char*)pv, l);
                  return 0;
                }
              }
            // END AttributeTypeAndValue
          // END rdnSequence
        }
      // END subject
    // END TBSCertificate
  // END Certificate
  return -1;
}
