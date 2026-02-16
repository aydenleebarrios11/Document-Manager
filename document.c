#include "document.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int init_document(Document *doc, const char *name) {
  if (doc == NULL || name == NULL || strlen(name) > MAX_STR_SIZE) {
    return FAILURE;
  }
  strcpy(doc->name, name);
  doc->number_of_paragraphs = 0;
  return SUCCESS;
}

int reset_document(Document *doc) {
  if (doc == NULL) {
    return FAILURE;
  }
  doc->number_of_paragraphs = 0;
  return SUCCESS;
}

int print_document(Document *doc) {
  int i;
  int j;
  Paragraph *p;
  if (doc == NULL) {
    return FAILURE;
  }
  printf("Document name: \"%s\"\n", doc->name);
  printf("Number of Paragraphs: %d\n", doc->number_of_paragraphs);
  for (i = 0; i < doc->number_of_paragraphs; i++) {
    p = &doc->paragraphs[i];
    for (j = 0; j < p->number_of_lines; j++) {
      printf("%s\n", p->lines[j]);
    }
    if (p->number_of_lines > 0 && i < doc->number_of_paragraphs - 1) {
      printf("\n");
    }
  }
  return SUCCESS;
}

int add_paragraph_after(Document *doc, int paragraph_number) {
  int i;
  if (doc == NULL || doc->number_of_paragraphs >= MAX_PARAGRAPHS ||
      paragraph_number > doc->number_of_paragraphs) {
    return FAILURE;
  }
  for (i = doc->number_of_paragraphs; i > paragraph_number; i--) {
    doc->paragraphs[i] = doc->paragraphs[i - 1];
  }
  (doc->paragraphs[paragraph_number]).number_of_lines = 0;
  doc->number_of_paragraphs++;
  return SUCCESS;
}

int add_line_after(Document *doc, int paragraph_number, int line_number,
                   const char *new_line) {
  int i;
  Paragraph *p = &doc->paragraphs[paragraph_number - 1];
  if (doc == NULL || paragraph_number > doc->number_of_paragraphs ||
      p->number_of_lines >= MAX_PARAGRAPH_LINES || new_line == NULL) {
    return FAILURE;
  }
  for (i = p->number_of_lines; i > line_number; i--) { 
    strcpy(p->lines[i], p->lines[i - 1]);
  }
  strcpy(p->lines[line_number], new_line);
  p->lines[line_number][MAX_STR_SIZE] = '\0';
  p->number_of_lines++;
  return SUCCESS;
}

int get_number_lines_paragraph(Document *doc, int paragraph_number,
                               int *number_of_lines) {
  if (doc == NULL || number_of_lines == NULL ||
      paragraph_number > doc->number_of_paragraphs) {
    return FAILURE;
  } else {
    *number_of_lines = doc->paragraphs[paragraph_number - 1].number_of_lines;
    return SUCCESS;
  }
}

int get_number_lines(Document *doc, int *number_of_lines) {
  int num = 0;
  int i;
  if (doc == NULL) {
    return FAILURE;
  }
  for (i = 0; i < doc->number_of_paragraphs; i++) {
    num += doc->paragraphs[i].number_of_lines;
  }
  *number_of_lines = num;
  return SUCCESS;
}

int append_line(Document *doc, int paragraph_number, const char *new_line) {
  Paragraph *p = &doc->paragraphs[paragraph_number - 1];
  if (doc == NULL || paragraph_number > doc->number_of_paragraphs ||
      p->number_of_lines >= MAX_PARAGRAPH_LINES || new_line == NULL) {
    return FAILURE;
  }
  strcpy(p->lines[p->number_of_lines], new_line);
  p->lines[p->number_of_lines][MAX_STR_SIZE] = '\0';
  p->number_of_lines++;
  return SUCCESS;
}

int remove_line(Document *doc, int paragraph_number, int line_number) {
  int i;
  Paragraph *p = &doc->paragraphs[paragraph_number - 1];
  if (doc == NULL || paragraph_number > doc->number_of_paragraphs ||
      doc->paragraphs[paragraph_number - 1].number_of_lines < line_number) {
    return FAILURE;
  }
  for (i = line_number - 1; i < p->number_of_lines - 1; i++) {
    strcpy(p->lines[i], p->lines[i + 1]);
  }
  p->number_of_lines -= 1;
  p->lines[p->number_of_lines][0] = '\0';
  return SUCCESS;
}

int load_document(Document *doc, char data[][MAX_STR_SIZE + 1],
                  int data_lines) {
  int cur = 0;
  int i;
  if (doc == NULL || data == NULL || data_lines == 0) {
    return FAILURE;
  }
  doc->number_of_paragraphs = 1;
  for (i = 0; i < data_lines; i++) {
    if (strlen(data[i]) == 0) {
      cur++;
      add_paragraph_after(doc, cur);
    } else {
      append_line(doc, cur + 1, data[i]);
    }
  }
  return SUCCESS;
}

int replace_text(Document *doc, const char *target, const char *replacement) {
  int i, j;
  char space[MAX_STR_SIZE + 1];
  char *index;
  int target_len = strlen(target);
  int rep_len = strlen(replacement);
  if (doc == NULL || target == NULL || replacement == NULL) {
    return FAILURE;
  }
  for (i = 0; i < doc->number_of_paragraphs; i++) {
    for (j = 0; j < doc->paragraphs[i].number_of_lines; j++) {
      char *line = doc->paragraphs[i].lines[j];
      while ((index = strstr(line, target)) != NULL) {
        strncpy(space, line, index - line);
        space[index - line] = '\0';
        strcat(space, replacement);
        strcat(space, index + target_len);
        strcpy(line, space);
        line = index + rep_len;
      }
    }
  }
  return SUCCESS;
}

int highlight_text(Document *doc, const char *target) {
  int i, j;
  char temp_line[MAX_STR_SIZE + 1];
  Paragraph *p;
  if (doc == NULL || target == NULL) {
    return FAILURE;
  }
  for (i = 0; i < doc->number_of_paragraphs; i++) {
    p = &doc->paragraphs[i];
    for (j = 0; j < p->number_of_lines; j++) {
      char *line = p->lines[j];
      char *pos;
      int len_target = strlen(target);
      temp_line[0] = '\0';
      pos = line;
      while ((pos = strstr(pos, target)) != NULL) {
        strncat(temp_line, line, pos - line);
        strcat(temp_line, HIGHLIGHT_START_STR);
        strcat(temp_line, target);
        strcat(temp_line, HIGHLIGHT_END_STR);
        pos += len_target;
        line = pos;
      }
      strcat(temp_line, line);
      strncpy(p->lines[j], temp_line, MAX_STR_SIZE);
    }
  }
  return SUCCESS;
}

int remove_text(Document *doc, const char *target) {
  int i;
  int j;
  char space[MAX_STR_SIZE + 1];
  Paragraph *p;
  if (doc == NULL || target == NULL) {
    return FAILURE;
  }
  for (i = 0; i < doc->number_of_paragraphs; i++) {
    p = &doc->paragraphs[i];
    for (j = 0; j < p->number_of_lines; j++) {
      char *line = p->lines[j];
      char *index;
      while ((index = strstr(line, target)) != NULL) {
        strncpy(space, line, index - line);
        space[index - line] = '\0';
        strcat(space, index + strlen(target));
        strcpy(p->lines[j], space);
      }
    }
  }
  return SUCCESS;
}

int load_file(Document *doc, const char *filename) {
  FILE *file = fopen(filename, "r");
  char line[MAX_STR_SIZE + 1];
  int cur = 0;
  if (doc == NULL || filename == NULL || file == NULL ||
      (doc->number_of_paragraphs + 1) > MAX_PARAGRAPHS) {
    return FAILURE;
  }
  if (doc->number_of_paragraphs == 0) {
    doc->number_of_paragraphs = 1;
  } else {
    add_paragraph_after(doc, cur);
  }
  while (fgets(line, sizeof(line), file) != NULL) {
    if (isspace(line[0])) {
      cur++;
      add_paragraph_after(doc, cur);
    } else {
      line[strlen(line) - 1] = '\0';
      append_line(doc, cur + 1, line);
    }
  }

  fclose(file);
  return SUCCESS;
}

int save_document(Document *doc, const char *filename) {
  FILE *file;
  int i, j;
  if (doc == NULL || filename == NULL) {
    return FAILURE;
  }
  file = fopen(filename, "w");
  if (file == NULL) {
    return FAILURE;
  }
  for (i = 0; i < doc->number_of_paragraphs; i++) {
    Paragraph *p = &doc->paragraphs[i];
    for (j = 0; j < p->number_of_lines; j++) {
      fprintf(file, "%s\n", p->lines[j]);
    }
    if (p->number_of_lines > 0 && i < doc->number_of_paragraphs - 1) {
      fprintf(file, "\n");
    }
  }
  fclose(file);
  return SUCCESS;
}
