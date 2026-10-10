"""Local PDF QA: requires pymupdf and Pillow; images are excluded from Git."""
from pathlib import Path
import pymupdf as fitz
import sys
sys.stdout.reconfigure(encoding='utf-8')
from PIL import Image, ImageDraw
root=Path(__file__).resolve().parents[1]
out=root/'docs/tmp'
out.mkdir(exist_ok=True)
targets=['https://alirezaaies.github.io/','https://github.com/alirezaaies',
         'https://www.linkedin.com/in/alirezakhajehvandi','https://www.youtube.com/@AlirezaAIES',
         'https://t.me/AlirezaAIES','https://www.instagram.com/alireza.aies']
for lang in ['english','persian']:
    doc=fitz.open(root/'docs'/lang/'main_report.pdf')
    print(lang,len(doc),'pages;',doc.metadata.get('author'))
    thumbs=[]
    for number,page in enumerate(doc):
        uris=[x.get('uri') for x in page.get_links()]
        assert all(t in uris for t in targets),(lang,number,uris)
        for link in page.get_links():
            assert page.rect.contains(link['from']), (lang,number,'link outside page')
        pix=page.get_pixmap(matrix=fitz.Matrix(.45,.45))
        thumbs.append(Image.frombytes('RGB',[pix.width,pix.height],pix.samples))
    cols=5
    w,h=thumbs[0].size
    sheet=Image.new('RGB',(cols*w,((len(thumbs)+cols-1)//cols)*(h+20)),'#cccccc')
    draw=ImageDraw.Draw(sheet)
    for number,im in enumerate(thumbs):
        x=(number%cols)*w; y=(number//cols)*(h+20)
        sheet.paste(im,(x,y)); draw.text((x+5,y+h),str(number+1),fill='black')
    sheet.save(out/(lang+'-contact.png'))
    for number in [0,2,5,len(doc)-1]:
        doc[number].get_pixmap(matrix=fitz.Matrix(1,1)).save(out/(lang+'-page-'+str(number+1)+'.png'))
print('PASS: all six author links on every page; metadata and link rectangles checked.')
